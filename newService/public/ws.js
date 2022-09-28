

var _ws = null
var _ws_open = false;
// var _send_message_callback_ptr = undefined;
// function _send_message_ws_callback(msg) {
//   if (msg.data instanceof Blob) {
//     console.log("binary??? HUH???: " + msg);
//   } // otherwise msg.data: DOMString object

//   if (_send_message_callback_ptr != undefined) {
//     _send_message_callback_ptr(msg);
//     _send_message_callback_ptr = undefined;
//   }
// }
// ws.onmessage = _send_message_ws_callback;
// function send_message_with_callback(msg, callback) {
//   ws.send(msg);
//   _send_message_callback_ptr = callback;
// }




////
//    PUBLIC
////
function ws_connected() { if (_ws == null) return false; return _ws_open }
function ws_initialize(url) {
  _ws = new WebSocket(url);
  _ws.onopen = function(msg) { _ws_open = true; }
  _ws.onclose = function(msg) { _ws_open = false;  }
  _ws.onerror = function(msg) { _ws_open = false; console.log("ws: error");  }
  _ws.onmessage = function(msg) { console.log("got msg: " + msg) }
}
function ws_send_message(msg) {
  if (!ws_connected()) return false;
  _ws.send(msg)
  return true
}