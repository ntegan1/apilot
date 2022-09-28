
var slider = null
var rangeValue = null
var interval1 = null
var sliderResetMillis = 60
var updated = true
var sliderResetDelta = 15
var mousedown = false
var touchdown = false
addEventListener("mousedown", function(event){
  // TODO: touch
	mousedown = true
		//var t = /touch/.test(event.type) ? event.targetTouches[0] : event.target;
		//t.classList.add('down');
}, true);
addEventListener("mouseup", function(event){
	mousedown = false
}, true);
addEventListener("touchstart", function(event){
  touchdown = true
		console.log('downtouch fired');
}, true);
addEventListener("touchend", function(event){
  touchdown = false
		console.log('duptouch fired');
}, true);
function setSlider(val) {
  var valString = String(Math.round(val))
  slider.value = valString
  rangeValue.innerText = valString
}
function getSlider() {
  return Number(slider.value)
}
function sliderTransform(slidervalue) {
  var a
  var b = new Uint8Array([0])
  if (slidervalue >= 0) {
    a = slidervalue
    b[0] = a /* 0 to 127 incl */
  } else { // negative, -128 to -1
    a = 127 + Math.abs(slidervalue)
    b[0] = a
  }
  console.log(a);
  var out = b[0]

  //var a = slidervalue + 127
  //var c = new Uint8Array([0])
  //var b = c[0] +
  //slidervalue > 0 ? slidervalue : 128 + Math.abs(slidervalue)


  
  //console.log("transforming, in: " + a + ", to out: " + b + ", or to c: " + c)
  return out
}
// https://blog.hubspot.com/website/html-slider
function sliderReset() {
  if (mousedown || touchdown) return
  var val = getSlider()
  if (val == 0) return
  if (val > 0) {
    if (val <= sliderResetDelta) { setSlider(0); return }
    setSlider(val - sliderResetDelta)
  } else if (val < 0) {
    if (val >= -sliderResetDelta) { setSlider(0); return }
    setSlider(val + sliderResetDelta)
  }
}
function onSliderLoad() {
  interval1 = null
  slider.oninput = function() {
    updated = true
    setSlider(getSlider())
    //globalSliderValue = this.value
  }
  slider.onchange = function() {
    updated = true
    setSlider(getSlider())
    //globalSliderValue = this.value
  }
  rangeValue = document.getElementById("rangeValue")
  rangeValue.innerText = String(slider.value)
  setInterval(sliderReset, sliderResetMillis)
  ws_initialize('wss://' + location.host)
  //location.protocol + "//" + location.host
  interval1 = setInterval(() => {
    if (ws_connected()) ws_send_message(sliderToByte(getSlider()))
  }, 1. / 15. * 1000.)

}
interval1 = setInterval(() => {slider = document.getElementById("slider1"); if (slider != null) clearInterval(interval1); onSliderLoad() }, 50)
var sliderToByte = (value) => { return new Uint8Array([sliderTransform(value)]) }
