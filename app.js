require('coffee-script/register')
var debug = require('debug')('papirus')
var Jimp = require('jimp')
var bitimage = require('./build/Release/bitimage')

var PaPiRus = require('./src/epd')

module.exports = PaPiRus

var screen = new PaPiRus({'auto': true})

screen_buffer = new bitimage.BitImage({
  "name":"screen",
  "width": screen.width(),
  "height": screen.height()
})

screen_buffer.AddObject1({
  "name": "temp",
  "x": 0,
  "y": 0,
  "size": 16,
  "align": "TOPLEFT",
})

screen_buffer.SetValue({
  "name": "temp",
  "value": "12",
})

screen_buffer.Draw(function (err, image) {

	if (err) {
		return debug(err)
	}

	screen.writeBuf(image, function (err, s) {
		debug(err)
	})
})
