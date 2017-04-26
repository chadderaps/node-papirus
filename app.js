require('coffee-script/register')
var debug = require('debug')('papirus')
var Jimp = require('jimp')
var bitscreen = require('./build/Debug/bitscreen')

var PaPiRus = require('./src/epd')

module.exports = PaPiRus

var screen = new PaPiRus({'auto': true, 'width': 15*8})

text1 = screen.addText('hi', 'A hi', {'x': 0, 'y': 0}, 16, 'TOPLEFT')
text2 = screen.addText('you', 'you', {'x': 4, 'y': 0}, 16, 'TOPLEFT')
text3 = screen.addText('and me', 'and me', {'x': 0, 'y': 0}, 16, 'BOTTOMRIGHT')

debug(text1)
debug('Setting align obj')
text2.AlignTo({'obj': text1, 'align': 'TOPRIGHT'})
debug('Setting align obj align')
debug('Printing text2')
debug(text2)

screen.display(function (err, obj) {
  debug(err)
})
