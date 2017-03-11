require('coffee-script/register')
var debug = require('debug')('papirus')
var Jimp = require('jimp')

var PaPiRus = require('./src/epd')

module.exports = PaPiRus

var screen = new PaPiRus({'auto': true})

screen.clear(function (err) {
	debug(err)
	debug('Completed screen clear')

	image = new Jimp(screen.width(), screen.height(), 0xFFFFFFFF, function (err, image) {
		if (err) {
			return
		}

		Jimp.loadFont(Jimp.FONT_SANS_32_BLACK).then(function (font) {
			image.print(font, 0, 0, 'Hello World')
			screen.display(image, function (err, screen) {
				debug(err)
				debug('Displayed Hello World')
			})
		}).catch(function (err) {
			debug(err)
		})
	})
})
