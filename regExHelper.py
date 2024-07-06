import re

addRegex = r'^\s*add\s*((?: +?"[^"]+?" *?))?((?: \(\s*[-]?\d+?\s*,\s*[-]?\d+?\s*\) *?)*)?$'
modRegex = r'^\s*mod\s*((?: +?"[^"]+?" *?))?((?: \(\s*[-]?\d+?\s*,\s*[-]?\d+?\s*\) *?)*)?$'
rmRegex = r'^\s*rm\s*((?: +?"[^"]+?" *?))?$'
ggRegex = r'^\s*gg\s*$'

def match(commandInfo, command):
    for reg in [addRegex, modRegex, rmRegex, ggRegex]:
        commandInfo = commandInfo or re.match(reg, command)
    return commandInfo

def getCoords(commandInfo):
    return re.findall(r'\(\s*[-]?\d+\s*,\s*[-]?\d+\s*\)', commandInfo.group(2))