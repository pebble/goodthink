module.exports = function(minified) {
  var clayConfig = this;
  var $ = minified.$;

  function changeHourlyPopups() {
    if(this.get() === '0'){
      clayConfig.getItemByMessageKey('hourly_survey_start_time').show();
      clayConfig.getItemByMessageKey('hourly_survey_end_time').show();
    }
    else {
      clayConfig.getItemByMessageKey('hourly_survey_start_time').hide();
      clayConfig.getItemByMessageKey('hourly_survey_end_time').hide();
    }
  }

  function changeEnableApp() {
    if (this.get()) {
      clayConfig.getItemByMessageKey('hourly_survey_start_time').enable();
      clayConfig.getItemByMessageKey('hourly_survey_end_time').enable();
      clayConfig.getItemByMessageKey('daily_survey_time').enable();
      clayConfig.getItemByMessageKey('snooze_option').enable();
      clayConfig.getItemByMessageKey('study_duration').enable();
      clayConfig.getItemByMessageKey('hourly_popups').enable();
      clayConfig.getItemById('optin-text').hide();
    } else {
      clayConfig.getItemByMessageKey('hourly_survey_start_time').disable();
      clayConfig.getItemByMessageKey('hourly_survey_end_time').disable();
      clayConfig.getItemByMessageKey('daily_survey_time').disable();
      clayConfig.getItemByMessageKey('snooze_option').disable();
      clayConfig.getItemByMessageKey('study_duration').disable();
      clayConfig.getItemByMessageKey('hourly_popups').disable();
      clayConfig.getItemById('optin-text').show();
    }
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var enableToggle = clayConfig.getItemByMessageKey('optin');
    changeEnableApp.call(enableToggle);
    enableToggle.on('change', changeEnableApp);

    var hourly_popups = clayConfig.getItemByMessageKey('hourly_popups');
    changeHourlyPopups.call(hourly_popups);
    hourly_popups.on('change', changeHourlyPopups);
  });

};