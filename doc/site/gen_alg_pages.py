#!/usr/bin/env python2

import glob
import os.path
import struct

try:
    import om
except:
    print "You need the openModeller SWIG Python module (om.py) installed to generate all pages!"
    exit(1)

# You'll need this instance for sure...
mod = om.OpenModeller()

# Loop over algorithms to collect their names
algList = mod.availableAlgorithms()

strAlgList = ""

ids = []
for a in algList:
    ids.append(a.id)

algs_file = open('algorithms.t2t', 'w')
algs_file.write("\n\n\n%!target: html\n")
algs_file.write('- Available Algorithms')

ids.sort()

i = 0
sep = ''
for a in ids:

    html = a.lower() + ".html"

    output = os.path.join("algorithms", html)

    # open file for output
    print "writing " + output
    o = open(output, 'w')

    alg = mod.algorithmMetadata(a)

    algs_file.write("\n"+' - ['+ alg.name +' algorithms/'+html+']')

    ####### generate HTML #######
    # header
    o.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 '
            'Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n')

    o.write('<head>'
            '<meta http-equiv="Content-Type" '
            'content="text/html;charset=utf-8">\n'
            '<link rel="stylesheet" type="text/css" href="../om.css">')
    o.write('<title>{0}</title></head><body>\n'.format(alg.name))

    # name
    o.write('<h2>{0}</h2>\n'.format(alg.name))
    o.write('<p>openModeller id: {0}</p>'.format(a))

    # developers
    o.write('<p><b>Current version:</b> {0}'.format(alg.version))
    o.write('&nbsp;&nbsp;&nbsp;&nbsp;<b>Developer(s):</b> {0}</p>\n'.format(alg.code_author.decode("iso-8859-1").encode('utf8')))

    # accepts categorical maps
    o.write('<p><b>Accepts Categorical Maps:</b> {0}</p>'.
            format("yes" if int(alg.categorical) else "no"))

    # require absence points
    o.write('<p><b>Requires absence points:</b> {0}</p>'.
            format("yes" if int(alg.absence) else "no")) 

    # authors
    o.write('<p><b>Author(s):</b> {0}</p>'.format(alg.author.decode("iso-8859-1").encode('utf8'))) 

    # description
    o.write('<h3>Description</h3>\n')
    o.write('<p>{0}</p>\n'.format(alg.description.decode("iso-8859-1").encode('utf8')))

    # bibliography
    if alg.biblio:
        o.write('<h3>Bibliography</h3>\n')
        o.write('<p>{0}</p>\n'.format(alg.biblio.decode("iso-8859-1").encode('utf8')))

    # parameters
    o.write('<h3>Parameters</h3>\n')
    j = 0
    parameters = alg.getParameterList()

    if len(parameters) > 0:
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
            o.write('<p>openModeller id: {0}</p>'.format(param.id))

            o.write('<p>{0}</p>\n'.format(param.description))
            p_type = int(param.type)
            p_type_name = '?'
            if p_type == 0:
                p_type_name = 'integer'
            elif p_type == 1:
                p_type_name = 'real'
            elif p_type == 2:
                p_type_name = 'string'
            o.write('<p><b>Data type:</b> {}&nbsp;&nbsp;'.format(p_type_name))
            if p_type < 2:
                o.write('<b>Domain:</b> [{}, {}]&nbsp;&nbsp;'.format(min, max))
            o.write('<b>Typical value:</b> {}</p>\n'.format(param.typical))
    else:
        o.write('<p>No parameters</p>\n')
        
    # models
    model = 'algorithms/' + a.lower()
    mfile = model + '-meta.txt'
    fglob = model + '-*.png'
    dglob = model + '-*.txt' 

    figs = glob.glob(fglob)
    figs.sort()
    descs = glob.glob(dglob)
    descs.sort()

    if not figs: # doesn't include Models if there are no figures
        i = i + 1
        continue

    o.write('<hr><h3>Sample models</h3>\n')

    if os.path.isfile(mfile):
        mf = open(mfile, 'r')
        meta = mf.read()
        o.write('<p>'+meta+'</p>')

    o.write('<table id="models" cellspacing="10">')

    images = ''
    captions = ''
    img_width = ''

    j = 1
    for f, d in zip(figs, descs):
        if j == 1:
            images = '<tr>'
            captions = '<tr>'
        elif ((j-1) % 3 == 0):
            o.write(images + '</tr>' + captions + '</tr>')
            images = '<tr>'
            captions = '<tr>'
            
        # read description
        dd = open(d, 'r')
        description = dd.readline()

        if img_width == '':
            print 'Opening'+f
            imgf = open(f, 'r')
            img_data = imgf.read()
            # Is PNG?
            if (img_data[:8] == '\211PNG\r\n\032\n'and (img_data[12:16] == 'IHDR')):
                w, h = struct.unpack('>LL', img_data[16:24])
                img_width = ' width="'+str(w)+'"'

        images = images + '<td'+img_width+'><img src="' + "../" + f +'" alt="niche"/></td>'
        captions = captions + '<td valign="top">'+description.rstrip()+'</td>'
        j = j + 1

    o.write(images + '</tr>' + captions + '</tr></table>')
    o.write('</body></html>\n')

    i = i + 1
    o.close()

algs_file.close()
