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

bitimage.addchar(buf, 'a', {'size': 16, 'x': 10, 'y': 1}, function (err, image) {
  if (err) {
    console.log(err)
    return
  }

  debug(image);


})
