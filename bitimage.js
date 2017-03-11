var bitimage = require('./build/Release/bitimage')
var Jimp = require('jimp')
var debug = require('debug')('papirus')
var fs = require('fs')

console.log(bitimage.hello())

var buf = new Buffer(100).fill(0xFF);
console.log(buf)

bitimage.convert(buf, function (err, val) {
  if (err) {
    console.log('Got Error')
    console.log(err)
    return
  }

  console.log(val)
})

var image = new Jimp(200, 96, 0xFFFFFFFF, function (err, image) {
	if (err) {
		return
	}

	Jimp.loadFont(Jimp.FONT_SANS_32_BLACK).then(function (font) {
		image.print(font, 0, 0, 'Hello World')
    bitimage.convert(image.bitmap.data, function (err, data) {
      debug(data)
      fs.writeFile('./debug.bmp', data, { 'encoding': 'binary' }, function (err) {
        debug("Wrote bmp file")
      })
    })
	}).catch(function (err) {
		debug(err)
	})
})
