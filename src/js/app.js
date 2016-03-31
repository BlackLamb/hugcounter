var totalHugsNum = window.localStorage.getItem('TotalHugsNum') ? window.localStorage.getItem('TotalHugsNum') : 1000;

Pebble.addEventListener('showConfiguration', function() {
	var url = 'https://dl.dropboxusercontent.com/spa/id55snvxb28xi4a/hugcounter/index.html?totalhugs=' + totalHugsNum;
	
	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
	var configData = JSON.parse(decodeURIComponent(e.response));
	
	var dict = {
		'AppTotalHugsNum': configData.total_hugs_num,
		'AppResetOnSave': configData.reset_on_save
	};
	totalHugsNum = configData.total_hugs_num
	window.localStorage.setItem('TotalHugsNum', totalHugsNum);
	//console.log(JSON.stringify(configData));
	
	Pebble.sendAppMessage(dict, function() {
		console.log('Config data sent successfully!');
	}, function(e) {
		console.log('Error sending config data!');
	});
});