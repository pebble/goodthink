var OWMWeather = function() {

  this._apiKey = 'ad2ae219d9f158f1f1ef54c4cf088cb6';

  this._xhrWrapper = function(url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
  }

  this.getWeather = function(pos, successCb) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + this._apiKey;
    // console.log('owm-weather: Location success. Contacting OpenWeatherMap.org...');

    this._xhrWrapper(url, 'GET', function(responseText) {
      // console.log('owm-weather: Got API response!');
      if(responseText.length > 100) {
        if(successCb)
          successCb(JSON.parse(responseText));
      } else {
        // console.log('owm-weather: API response was bad. Wrong API key?');
        Pebble.sendAppMessage({ 'OWMWeatherAppMessageKeyBadKey': 1 });
      }
    });
    return true;
  };
}

module.exports = OWMWeather;