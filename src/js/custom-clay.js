module.exports = function(minified) {
	var clayConfig = this;
	
	clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
		var resetToggle = clayConfig.getItemByMessageKey('AppResetOnSave');
		resetToggle.set(false);
	});
};