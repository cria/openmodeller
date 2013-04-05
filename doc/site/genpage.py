#!/usr/bin/env python2

import om

# You'll need this instance for sure...
mod = om.OpenModeller()

# Loop over algorithms to collect their names
algList = mod.availableAlgorithms()

strAlgList = ""

#for i in range(0, len(algList)):
#    #alg = algList[i]
#    print algList[i].name

id = []
html = []
for a in algList:
    id.append(a.id)

    output = a.id.lower() + ".html"
    html.append(output)

    print a.id, output

print "--"
i = 0
for a in id:
    # open file for output
    print "writing " + html[i]
    o = open(html[i], 'w')

    alg = mod.algorithmMetadata(a)

    ####### generate HTML #######
    # header
    o.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 '
            'Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n')

    o.write('<head>'
            '<meta http-equiv="Content-Type" '
            'content="text/html;charset=utf-8">\n')
    o.write('<title>{0}</title></head><body>\n'.format(alg.name))

    # name
    o.write('<h2>{0}</h2>\n'.format(alg.name))

    # developers
    o.write('<p><h3>Developers</h3>\n')
    o.write('<p><b>Developer(s)</b>: {0}</p>\n'.format(alg.code_author))

    # accepts categorical maps
    o.write('<p><b>Accepts Categorical Maps:</b> {0}</p>'.
            format("yes" if int(alg.categorical) else "no"))

    # require absence points
    o.write('<p><b>Requires absence points:</b> {0}</p>'.
            format("yes" if int(alg.absence) else "no")) 

    # bibliography
    o.write('<h3>Bibliography</h3>\n')
    o.write('<p>{0}</p>\n'.format(alg.biblio))

    # description
    o.write('<h3>Description</h3>\n')
    o.write('<p>{0}</p>\n'.format(alg.description))

    #parameters
    o.write('<h3>Parameters</h3>\n')
    j = 0
    parameters = alg.getParameterList()
    for param in parameters:
        if param.has_min:
            min = param.min_val
        else:
            min = "oo"

        if param.has_max:
            max = param.max_val
        else:
            max = "oo"

        o.write('<p><u><b>{0}</b></u></p>\n'.format(param.name))

        o.write('<p><b>Domain:</b> [{}, {}]<br>\n'.format(min, max))
        o.write('<b>Data type:</b> {}<br>\n'.
                format("real" if int(param.type) else "integer"))
        o.write('<b>Typical value:</b> {}<br></p>\n'.format(param.typical))
        o.write('<p><b>Meaning:</b> {0}</p>\n'.format(param.overview))
        print param.id, param.name, min
        
    i = i + 1
    o.close()
