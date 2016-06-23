var totalHugsNum = localStorage.getItem('TotalHugsNum') ? localStorage.getItem('TotalHugsNum') : 1000;
var showSecondsBol = localStorage.getItem('ShowSeconds') ? localStorage.getItem('ShowSeconds') : 1;

Pebble.addEventListener('showConfiguration', function() {
	var url = 'https://dl.dropboxusercontent.com/spa/id55snvxb28xi4a/hugcounter/index.html?totalhugs=' + totalHugsNum + '&showseconds=' + showSecondsBol;
	
	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
	var configData = JSON.parse(decodeURIComponent(e.response));
	
	var dict = {
		'AppTotalHugsNum': configData.total_hugs_num,
		'AppResetOnSave': configData.reset_on_save,
		'AppShowSeconds': configData.show_seconds
	};
	totalHugsNum = configData.total_hugs_num;
	localStorage.setItem('TotalHugsNum', totalHugsNum);
	showSecondsBol = configData.show_seconds;
	localStorage.setItem('ShowSeconds', showSecondsBol);
	//console.log(JSON.stringify(configData));
	
	Pebble.sendAppMessage(dict, function() {
		console.log('Config data sent successfully!');
	}, function(e) {
		console.log('Error sending config data!');
	});
});