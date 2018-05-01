
var EventUtil = {
	addEvent: function(element, type, hander) {
		if(element.addEventListener) {
			element.addEventListener(type, hander, false);
		} else if (element.attachEvent) {
			element.attachEvent('on' + type, hander);
		} else {
			element['on' + type] = hander;
		}
	}

	removeEvent: function(element, type, hander) {
		if(element.removeEventListener) {
			element.removeEventListener(type, hander, false);
		} else if(element.detachEvent) {
			element.detachEvent('on' + type);
		} else {
			element['on' + type] = hander;
		}
	
};

var btn = document.getElementById('btn');
EventUtil.addEvent(btn, 'click', hander);