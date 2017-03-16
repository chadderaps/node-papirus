require('coffee-script/register')
var debug = require('debug')('papirus')
var Jimp = require('jimp')
var bitimage = require('./build/Release/bitimage')

var PaPiRus = require('./src/epd')

module.exports = PaPiRus

var screen = new PaPiRus({'auto': true})

var buf = new Buffer(100)

bitimage.addchar(buf, 'd', {'size': 16}, function (err, image) {

	if (err) {
		return debug(err)
	}

	screen.writeBuf(image, function (err, s) {
		debug(err)
	})
})
