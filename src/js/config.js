module.exports = [
	{
		"type":"heading",
		"defaultValue":"Configure Hug Counter",
		"size":2
	},
	{
		"type":"section",
		"items": [
			{
				"type":"heading",
				"defaultValue":"Instructions"
			},
			{
				"type":"text",
				"defaultValue":"<p>Welcome to Hug Counter</p><br/><p>Change the the target number of hugs using the slider, valid amounts are 10 - 9990.</p><br/><p>You may reset the number of hugs that have been counted when changing the target by checking the checkbox.</p><br/><p>To count hugs click the middle select button on the watch. To reset your count hold down the down button for two seconds.</p><br/>"
			}
		]
	},
	{
		"type":"section",
		"items": [
			{
				"type":"heading",
				"defaultValue":"Settings"
			},
			{
				"type":"slider",
				"messageKey":"AppTotalHugsNum",
				"label":"Total Hugs",
				"defaultValue":1000,
				"min":10,
				"max":9990,
				"step":10
			},
			{
				"type":"toggle",
				"messageKey":"AppResetOnSave",
				"label":"Reset Count on Save?",
				"defaultValue":false
			},
			{
				"type":"toggle",
				"messageKey":"AppShowSeconds",
				"label":"Show Seconds?",
				"defaultValue":true
			},
			{
				"type": "select",
				"messageKey": "AppBackground",
				"label": "Background",
				"description": "Select Background Image",
				"defaultValue": "IMAGE_HUGS_BACKGROUND",
				"options" : [
					{
						"label": "Hugs",
						"value": "IMAGE_HUGS_BACKGROUND"
					},
					{
						"label": "Image 2",
						"value": "IMAGE_BACKGROUND_2"
					},
					{
						"label": "Image 3",
						"value": "IMAGE_BACKGROUND_3"
					},
					{
						"label": "Image 4",
						"value": "IMAGE_BACKGROUND_4"
					}
				]
			}
		]
	},
	{
		"type":"submit",
		"defaultValue":"Save"
	}
];