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

bitimage.addchar(buf, '12', {'size': 16, 'x': 0, 'y': 0}, function (err, image) {
  if (err) {
    console.log(err)
    return
  }
  let count = 25;

  for (let i = 0; i < image.length; i+=count) {
    debug(image.slice(i,i+count));
  }
})

var obj = new bitimage.BitImage({
  "name": "stuff",
  "width": 200,
  "height": 96,
})

obj.AddObject1({
  "name": "and that",
  "x": 0,
  "y": 0,
  "size": 16,
  "align": "LEFT"
})

debug(obj)

debug(obj.GetObject({
  "name": "and that"
}))

debug(obj.SetValue({
  "name": "and that",
  "value": "1 2"
}))

debug(obj)

debug(obj.GetObject({
  "name": "and that"
}))

obj.Draw(function (err, image) {
  if (err) {
    debug("Got Error")
    return debug(err)
  }

  let count = 25;

  console.log(image.length);

  for (let i = 0; i < image.length; i+=count) {
    debug(image.slice(i,i+count));
  }

})
