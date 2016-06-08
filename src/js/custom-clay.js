module.exports = function(minified) {
  var clayConfig = this;
  var $ = minified.$;

  function changeEnableApp() {
    if (this.get()) {
      clayConfig.getItemByAppKey('hourly_survey_start_time').enable();
      clayConfig.getItemByAppKey('hourly_survey_end_time').enable();
      clayConfig.getItemByAppKey('daily_survey_time').enable();
      clayConfig.getItemByAppKey('snooze_option').enable();
      clayConfig.getItemById('optin-text').hide();
    } else {
      clayConfig.getItemByAppKey('hourly_survey_start_time').disable();
      clayConfig.getItemByAppKey('hourly_survey_end_time').disable();
      clayConfig.getItemByAppKey('daily_survey_time').disable();
      clayConfig.getItemByAppKey('snooze_option').disable();
      clayConfig.getItemById('optin-text').show();
    }
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var enableToggle = clayConfig.getItemByAppKey('optin');
    changeEnableApp.call(enableToggle);
    enableToggle.on('change', changeEnableApp);
  });

};