fs = require 'fs'
path = require 'path'
debug = require('debug') 'papirus'
Jimp = require 'jimp'
bitimage = require '../build/Release/bitimage'

module.exports =
class EPD

  constructor: (params = {}) ->

    defaultParams = {
      epd_path: '/dev/epd'
      width: 200
      height: 96
      panel: 'EPD 2.0'
      cog: 0
      film: 0
      auto: false
    }

    @settings = Object.assign {}, defaultParams, params

    @loadVersion()
    @loadPanel()


  loadVersion: () ->
    versionFile = path.join @settings.epd_path, 'version'
    versionInfo = fs.readFileSync versionFile, { encoding: 'utf8' }
    debug versionInfo
    @version = versionInfo.split('\n')[0]

  loadPanel: () ->
    panelFile = path.join @settings.epd_path, 'panel'
    panelInfo = fs.readFileSync panelFile, { encoding: 'utf8' }
    panelLines = panelInfo.split '\n'

    patt = ///^([A-Za-z]+)\s+(\d+\.\d+)\s+(\d+)x(\d+)\s+COG\s+(\d+)\s+FILM\s+(\d+)\s*$///

    panelData = panelLines[0].match patt

    throw new Error('invalid panel string') if not panelData

    @settings.panel = "#{panelData[1]} #{panelData[2]}"
    @settings.width = parseInt panelData[3]
    @settings.height = parseInt panelData[4]
    @settings.cog = parseInt panelData[5]
    @settings.film = parseInt panelData[6]

    throw new Error('invalid panel geometry') if @settings.width < 1 or @settings.height < 1


  width: () ->
    return @settings.width

  height: () ->
    return @settings.height

  numPixels: () ->
    return @width() * @height()

  numBytes: () ->
    return @numPixels() / 8

  display: (image, callback) ->

    image.greyscale()

    if @width() is not image.bitmap.width or @height() is not image.bitmap.height
      err = Error('Image does not fit screen')
      return callback err, null

    debug 'start'
    #bitimage.convert image.bitmap.data, (err, data) =>
    @convert image, (err, data) =>
      debug 'done convert'
      writePath = path.join @settings.epd_path, 'LE', 'display_inverse'
      fs.writeFile writePath, data, {
        encoding: 'binary'
      }, (err) =>
        if not err? and @settings.auto
          debug 'done writing image'
          return @update(callback)
        else
          return callback err, @

    return

  writeBuf: (buf, callback) ->

    for byte in buf
      debug byte

    writePath = path.join @settings.epd_path, 'LE', 'display_inverse'
    fs.writeFile writePath, buf, {
      encoding: 'binary'
    }, (err) =>
      if not err? and @settings.auto
        debug 'done writing image'
        return @update(callback)
      else
        return callback err, @


  convert: (image, callback) ->
    arr = new Array(@numBytes()).fill 0x00
    data = image.bitmap.data

    image.scan 0, 0, @width(), @height(), (x, y, idx) =>
      pixel = data[idx]
      #debug "#{x}, #{y}, #{idx}"
      if pixel != data[idx+1] and pixel != data[idx+2]
        debug 'Got non matching pixels'
      bit = if pixel > 250 then 1 else 0

      pixelIdx = y * @width() + x
      arrIdx = Math.floor pixelIdx / 8
      bitIdx = pixelIdx % 8

      arr[arrIdx] = arr[arrIdx] | bit << (7-bitIdx)

    buf = new Buffer(arr)

    callback null, buf


  clear: (callback) ->
    @command 'C', callback

  update: (callback) ->
    @command 'U', callback

  partialUpdate: (callback) ->
    @command 'P', callback

  command: (cmd, callback) ->
    cmdPath = path.join @settings.epd_path, 'command'
    fs.writeFile cmdPath, cmd, {
      encoding: 'binary'
    }, (err) =>
      return callback err

    return
