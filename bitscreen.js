var bitscreen = require('./build/Debug/bitscreen')
var Jimp = require('jimp')
var debug = require('debug')('papirus')
var fs = require('fs')

console.log(bitscreen.hello())

var buf = new Buffer(100).fill(0xFF);
console.log(buf)

bitscreen.convert(buf, function (err, val) {
  if (err) {
    console.log('Got Error')
    console.log(err)
    return
  }

  console.log(val)
})

bitscreen.addchar(buf, '12', {'size': 16, 'x': 0, 'y': 0}, function (err, image) {
  if (err) {
    console.log(err)
    return
  }
  let count = 25;

  for (let i = 0; i < image.length; i+=count) {
    debug(image.slice(i,i+count));
  }
})

var obj = new bitscreen.BitScreen({
  "name": "stuff",
  "width": 200,
  "height": 96,
})

temp_display = obj.AddObject({
  "name": "temp",
  "x": 0,
  "y": 0,
  "size": 16,
  "align": "TOPLEFT"
})

temp_display.value = "12"
debug("TempDisplay")
debug(temp_display);

temp_label = obj.AddObject({
  "name": "temp_label",
  "x": 0,
  "y": 0,
  "size": 16,
  "align": "TOPLEFT",
  "alignToObj": temp_display
})

debug(temp_label)

debug(obj.GetObject({
  "name": "temp"
}))

temp_label.value = "°F"

obj.Draw(function (err, image) {
  if (err) {
    debug("Got Error")
    return debug(err)
  }

  let count = 25;
  let draw = 10;

  console.log(image.length);

  for (let i = 0; i < 500; i+=count) {
    bits = ''
    for (let j = 0; j < draw; j++) {
      b = image[i+j];
      for (let k = 0; k < 8; k++) {
        if (b & (1 << (7-k))) {
          bits += '.'
        } else {
          bits += ' '
        }
      }
    }
    debug(bits)
  }

})
