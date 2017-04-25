fs = require 'fs'
path = require 'path'
debug = require('debug') 'papirus'
Jimp = require 'jimp'
bitscreen = require '../build/Debug/bitscreen'

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

    @dummy = process.env.DUMMY_PAPIRUS?

    @loadVersion()
    @loadPanel()

    @screen = new bitscreen.BitScreen {
      name: "PaPirus"
      width: @width()
      height: @height()
      dpi: 96
    }


  loadVersion: () ->
    if @dummy
      @version = '1.0.0'
    else
      versionFile = path.join @settings.epd_path, 'version'
      versionInfo = fs.readFileSync versionFile, { encoding: 'utf8' }
      debug versionInfo
      @version = versionInfo.split('\n')[0]

  loadPanel: () ->

    if not @dummy
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

  addText: (objName, text, start, fontSize, align) ->
    obj = @screen.AddObject {
      name: objName
      x: start.x
      y: start.y
      align: align
      size: fontSize
    }

    obj.value = text

    return obj

  width: () ->
    return @settings.width

  height: () ->
    return @settings.height

  numPixels: () ->
    return @width() * @height()

  numBytes: () ->
    return @numPixels() / 8

  display: (callback) ->

    debug 'start'
    #bitimage.convert image.bitmap.data, (err, data) =>
    @screen.Draw (err, data) =>
      debug 'screen buffer built'
      @writeBuf data, callback

    return

  writeBuf: (buf, callback) ->

    if @dummy
      @displayBuf buf
      return callback null, @

    writePath = path.join @settings.epd_path, 'LE', 'display_inverse'
    fs.writeFile writePath, buf, {
      encoding: 'binary'
    }, (err) =>
      if not err? and @settings.auto
        debug 'done writing image'
        return @update(callback)
      else
        return callback err, @

  displayBuf: (buf) ->
    row = 50
    col = 15

    for i in [0..row-1]
      bits = ''
      for j in [0..col-1]
        b = buf[i*25+j]
        for k in [0..7]
          if (b & (1 << (7-k)))
            bits += '.'
          else
            bits += ' '

      debug(bits)

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
