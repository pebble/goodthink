var appinfo = require('appinfo.json');

var Clay = require('lib/clay');
var clayConfig = require('config.json');
var customClay = require('custom-clay');
var clay = new Clay(clayConfig, customClay, {autoHandleEvents: false});

var ajax = require('lib/ajax');

var OWMWeather = require('lib/owm_weather.js')
var owmWeather = new OWMWeather();

var coordinates = null;
var weather = null;
var timeline_token = "";

function success_weather(weather_) {
  weather = weather_;
  console.log(JSON.stringify(weather));
}

Pebble.addEventListener('ready', function(e) {
  console.log(appinfo.longName + " " + appinfo.versionLabel + " ready !");

  Pebble.getTimelineToken(function(token) {
    timeline_token = token;
  }, function(error) {
    timeline_token = "";
  });

  function success(pos) {
    console.log('location ok');
    owmWeather.getWeather(pos, success_weather);
    coordinates = pos.coords;
  }

  function error(err) {
    console.log('location error (' + err.code + '): ' + err.message);
  }

  navigator.geolocation.getCurrentPosition(success, error, {enableHighAccuracy: true});
});

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) { return; }

  var settings = clay.getSettings(e.response);

  var time = settings['hourly_survey_start_time'].split(':');
  settings['hourly_survey_start_time'] = parseInt(time[0])*60 + parseInt(time[1]);

  time = settings['hourly_survey_end_time'].split(':');
  settings['hourly_survey_end_time'] = parseInt(time[0])*60 + parseInt(time[1]);

  time = settings['daily_survey_time'].split(':');
  settings['daily_survey_time'] = parseInt(time[0])*60 + parseInt(time[1]);

  console.log(JSON.stringify(settings));

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(settings, function(e) {
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));
  });
});

function sendPackets(packets, id, cache) {
  console.log(JSON.stringify(packets[id]));
  ajax(
    {
      url : 'https://wellness-api.getpebble.com/v1',
      method : 'post',
      type : 'json',
      data : packets[id]
    },
    function(data, status, request) {
      if(id > 0) {
        sendPackets(packets, id-1, cache);
      }
      else {
        console.log("cachedPackets " + JSON.stringify(cache));
        localStorage.setItem("cachedPackets", JSON.stringify(cache));
        Pebble.sendAppMessage({'send_response': 1});
      }
    },
    function(error, status, request) {
      console.log('The ajax request failed: ' + error);
      console.log("Caching data for next attempt");

      cache.push(packets[id]);

      if(id > 0) {
        sendPackets(packets, id-1, cache);
      }
      else {
        console.log("cachedPackets " + JSON.stringify(cache));
        localStorage.setItem("cachedPackets", JSON.stringify(cache));
        Pebble.sendAppMessage({'send_response': 1});
      }
      
    }
  );
}

Pebble.addEventListener('appmessage', function(e) {

  e.payload.user            = Pebble.getAccountToken();
  e.payload.timeline_token  = timeline_token;
  e.payload.timezone        = new Date().getTimezoneOffset();

  if(e.payload.is_old){
    delete e.payload.is_old;
  }
  else {
    if(coordinates)
      e.payload.coordinates = {'latitude' : coordinates.latitude, 'longitude' : coordinates.longitude};
    if(weather)
      e.payload.weather = weather;
  }

  // send cached data:
  var packets = localStorage.getItem("cachedPackets");
  if (packets) {
    packets = JSON.parse(packets);
  }
  else {
    packets = [];
  }
  packets.push(e.payload);

  // Send data
  sendPackets(packets, packets.length-1, []);
});
