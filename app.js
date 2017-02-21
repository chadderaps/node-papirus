require('coffee-script/register')
var debug = require('debug')('papirus')

var PaPiRus = require('./src/epd')

module.exports = PaPiRus

var screen = new PaPiRus()

screen.clear(function (err) {
	debug(err)
	debug('Completed screen clear')
})
