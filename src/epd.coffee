fs = require 'fs'
path = require 'path'
debug = require('debug') 'papirus'

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


  display: (image, callback) ->

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
