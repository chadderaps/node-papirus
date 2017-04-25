var bitscreen = require('./build/Release/bitscreen')
var Jimp = require('jimp')
var debug = require('debug')('papirus')
var fs = require('fs')

console.log(bitscreen.hello())

var obj = new bitscreen.BitScreen({
  "name": "stuff",
  "width": 200,
  "height": 96,
  "dpi": 96,
})

debug(obj);

temp_display = obj.AddObject({
  "name": "temp",
  "x": 0,
  "y": 0,
  "size": 16,
  "align": "TOPLEFT"
})

temp_display.value = "hi"
debug("TempDisplay")
debug(temp_display);

temp_label = obj.AddObject({
  "name": "temp_label",
  "x": 4,
  "y": 0,
  "size": 16,
  "align": "TOPLEFT",
  "alignTo": {
    "obj": temp_display,
    "align": "BOTTOMRIGHT"
  }
})

debug(temp_label)

temp_debug = temp_label.alignTo.obj

debug(obj.GetObject({
  "name": "temp"
}))

temp_label.value = "you"

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
