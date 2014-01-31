#!/usr/bin/env python

#################################################################################
# This program was created for the BioVeL project to test an OMWS 2.0 endpoint
# through the BiodiversityCatalogue (http://www.biodiversitycatalogue.org).
#
# To run it you need Python >= 2.4 (but very likely < 3.0) and the suds library
# that can be downloaded from:
# https://fedorahosted.org/releases/s/u/suds/python-suds-0.4.tar.gz
#
# The test runs all service operations, relying on the existence of the BIOCLIM
# algorithm and the following two layers on the server side:
#
# Minimum temperature during the coldest month:
layer1 = '/layers/en/terrestrial/climate/global/worldclim/present/bioclim/10arc-minutes/f6e850a0-04d9-11e1-be50-0800200c9a66'
# Precipitation during the driest month:
layer2 = '/layers/en/terrestrial/climate/global/worldclim/present/bioclim/10arc-minutes/3ed05d93-04da-11e1-be50-0800200c9a66'
#
# Since BiodiversityCatalogue test scripts don't take any arguments, a default
# oM Server endpoint is hard coded here. You need to replace it if you wish to
# use the same test for a different endpoint in BiodiversityCatalogue:
#
endpoint = 'http://modeller.cria.org.br/ws2/om'
#
# The program also accepts an --endpoint parameter if you wish to test another
# service URL outside BiodiversityCatalogue.
#################################################################################

try:
    import datetime, time, string, logging, urllib
    from urllib2 import URLError
    from optparse import OptionParser
    from xml.sax import SAXParseException
    
    started = time.time()

    # Handle arguments
    parser = OptionParser()
    parser.add_option("-e", "--endpoint", dest="endpoint", help="Service endpoint")

    (options, args) = parser.parse_args()

    if options.endpoint is not None:
        endpoint = options.endpoint

    # Special check for external suds module
    try:
        from suds.client import Client
        from suds import WebFault
        from suds.sax.element import Element
        from suds.plugin import MessagePlugin
    except ImportError:
        print 'Missing Python suds module!'
        exit(1)

    # Custom classes & functions
    class NullLogHandler(logging.Handler):
        "Custom log handler for suds to ignore log messages"
        def emit(self, record):
            pass

    class DumperPlugin( MessagePlugin ):
        "Custom SOAP plugin to dump XML messages"
        dreq = '?'
        dresp = '?'

        def sending( self, context ):
            "This method is called before the client sends the XML message"
            self.__class__.dreq = str( context.envelope )

        def received( self, context ):
            "This method is called when the client receives an XML message"
            self.__class__.dresp = str( context.reply )

    def check_status( client, ticket ):
        "Check job status"
        status = client.service.getProgress( ticket )
        return int( status )

    def get_suds_element( suds_obj, element_name ):
        "Build a suds Element based on a suds-encoded response variable"
        el = Element(element_name)
        for node in suds_obj:
            left = node[0]
            right = node[1]
            # Attributes start with _
            if left[:1] == '_':
                el.set( left[1:], str(right) )
            # Otherwise it's a subelement
            else:
                if type(right) is list:
                    for item in right:
                        sub = get_suds_element( item, str(left) )
                        el.append( sub )
                else:
                    sub = get_suds_element( right, str(left) )
                    el.append( sub )
        return el

    print "OMWS 2.0 test (v2)"
    print "=================="

    xml_ns = 'http://openmodeller.cria.org.br/xml/2.0'

    # General settings
    if '?' in endpoint:
        wsdl = endpoint + '&wsdl'
    else:
        wsdl = endpoint + '?wsdl'

    # Instantiate SOAP client
    print 'Checking endpoint:',endpoint
    soap_client = Client(wsdl, location=endpoint, prettyxml=True, plugins=[DumperPlugin()])
    soap_client.add_prefix('om', xml_ns)
    soap_client.add_prefix('soap', 'http://schemas.xmlsoap.org/soap/envelope/')

    # Ignore logs
    h = NullLogHandler()
    logging.getLogger('suds.client').addHandler(h)
    logging.getLogger('suds.umx.typed').addHandler(h)
    logging.getLogger('suds.plugin').addHandler(h)

    # Test configuration
    run_model = True
    run_model_test = True
    run_projection = True
    run_model_evaluation = True
    run_sample_points = True

    #####  PING
    ###################################
    try:
        status = soap_client.service.ping()
    except SAXParseException:
        print 'ping call failure: could not parse response (Internal Server Error?)'
        exit(2)
    except Exception, e:
        print 'ping call failure:',str(e)
        exit(2)

    if ( status <> 1 ):
        print 'Service unavailable!'
        exit(3)

    # If you need to see the XML messages:
    #print 'request:'
    #print DumperPlugin.dreq
    #print 'response:'
    #print DumperPlugin.dresp

    print 'Ping: OK'

    try:
        version = str( soap_client.last_received().root().getChild('Header').getChild('version').getText() )
        print 'Detected version:',version
    except Exception, e:
        pass

    #####  GET ALGORITHMS
    ###################################
    try:
        algs = soap_client.service.getAlgorithms()
    except Exception, e:
        print 'getAlgorithms call failure:',str(e)
        exit(4)

    num_algs = len( algs.Algorithms.Algorithm )
    if num_algs == 0:
        print 'No algorithms available!'
        exit(5)
    print 'Found',num_algs,'algorithms'
    found_bioclim = False
    for alg in algs.Algorithms.Algorithm:
        if alg._Id == 'BIOCLIM':
            found_bioclim = True
            break
    if not found_bioclim:
        print 'BIOCLIM algorithm not available!'
        exit(6)

    print 'GetAlgorithms: OK'

    #####  GET LAYERS
    ###################################
    try:
        group = soap_client.service.getLayers()
    except Exception, e:
        print 'getLayers call failure:',str(e)
        exit(7)

    # Non recursive tree traversal
    groups = [group]
    not_found_layer1 = True
    not_found_layer2 = True
    num_layers = 0
    while len(groups) > 0:
        group = groups.pop(0) # get first item
        # Inspect layers on that group
        if hasattr(group, 'Layer'):
            if type(group.Layer) is list:
                for layer in group.Layer:
                    num_layers += 1
                    if layer._Id == layer1:
                        not_found_layer1 = False
                    elif layer._Id == layer2:
                        not_found_layer2 = False
            else:
                num_layers += 1
                if group.Layer._Id == layer1:
                    not_found_layer1 = False
                elif group.Layer._Id == layer2:
                    not_found_layer2 = False
        # Load subgroups to be inspected in the next iterations
        if hasattr(group, 'LayersGroup'):
            if type(group.LayersGroup) is list:
                for g in group.LayersGroup:
                    groups.append( g )
            else:
                groups.append( group.LayersGroup )
    if num_layers == 0:
        print 'No layers available!'
        exit(8)
    print 'Found',num_layers,'layers'
    if not_found_layer1:
        print 'Layer 1 not found!'
        exit(9)
    if not_found_layer2:
        print 'Layer 2 not found!'
        exit(10)

    print 'GetLayers: OK'

    layers = [layer1, layer2]

    #####  CREATE MODEL
    ###################################
    if run_model:
        mod_params = Element('ModelParameters')
        mod_params.applyns( (None, xml_ns) )
        sampler = Element('Sampler')
        env = Element('Environment')
        # Layers
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        # Mask
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        sampler.append( env )
        # Points
        presence = Element('Presence')
        presence.set('Label', 'test_service.py')
        coord = Element('CoordinateSystem')
        coord.setText("GEOGCS['WGS84', DATUM['WGS84', SPHEROID['WGS84', 6378137.0, 298.257223563]], PRIMEM['Greenwich', 0.0], UNIT['degree', 0.017453292519943295], AXIS['Longitude',EAST], AXIS['Latitude',NORTH]]")
        presence.append( coord )
        p1 = Element('Point')
        p1.set('Id', '1')
        p1.set('X', '-68.85')
        p1.set('Y', '-11.15')
        presence.append( p1 )
        p2 = Element('Point')
        p2.set('Id', '2')
        p2.set('X', '-64.70')
        p2.set('Y', '-15.97')
        presence.append( p2 )
        sampler.append( presence )
        mod_params.append( sampler )
        # Algorithm parameters
        alg = Element('Algorithm')
        alg.set('Id', 'BIOCLIM')
        alg.set('Version', '0.2')
        params = Element('Parameters')
        param = Element('Parameter') 
        param.set('Id', 'StandardDeviationCutoff')
        param.set('Value', '0.8')
        params.append( param )
        alg.append( params )
        mod_params.append( alg )
        # Statistics parameters
        stat_param = Element('Statistics')
        conf_matrix = Element('ConfusionMatrix')
        conf_matrix.set('Threshold', 0.5)
        stat_param.append( conf_matrix )
        mod_params.append( stat_param )

        retries = 3
        while retries:
            try:
                ticket = soap_client.service.createModel( mod_params )
                if ticket is None:
                    print 'No ticket returned in createModel!'
                    exit(11)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(f)
                        exit(12)
                else:
                    print 'Web fault:',str(f)
                    exit(13)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(u)
                        exit(14)
                else:
                    print 'URL error:',str(f)
                    exit(15)
            except SAXParseException:
                print 'createModel call failure: could not parse response (Internal Server Error?)'
                exit(16)
            except Exception, e:
                print 'createModel call failure:',str(e)
                exit(16)

        print 'CreateModel: OK','( ticket',ticket,')'

        #####  GET PROGRESS
        ###################################

        # Keep running until job is finished
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    print 'Model creation aborted!'
                    exit(16)
                now = time.time()
                if now - start > 20*60:
                    print 'Service is taking too long to create model. Aborting.'
                    exit(17)
                time.sleep(5)
                progress = check_status( soap_client, ticket )
        except Exception, e:
            print 'getProgress call failure:',str(e)
            exit(18)

        print 'GetProgress: OK'

        #####  GET MODEL
        ###################################
        try:
            model = soap_client.service.getModel( ticket )
        except Exception, e:
            print 'getModel call failure:',str(e)
            exit(19)

        if not hasattr( model, 'SerializedModel' ):
            print 'No SerializedModel element!'
            exit(20)

        serialized_model = get_suds_element( model.SerializedModel.Algorithm, 'Algorithm' )

        print 'GetModel: OK'

        #####  GET LOG
        ###################################
        try:
            log = soap_client.service.getLog( ticket )
        except Exception, e:
            print 'getLog call failure: ', str(e)
            exit(21)

        print 'GetLog: OK'

    #####  TEST MODEL
    ###################################
    if run_model_test:
        test_params = Element('TestParameters')
        test_params.applyns( (None, xml_ns) )
        sampler = Element('Sampler')
        env = Element('Environment')
        # Layers
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        # Mask
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        sampler.append( env )
        # Points
        presence = Element('Presence')
        presence.set('Label', 'test_service.py')
        coord = Element('CoordinateSystem')
        coord.setText("GEOGCS['WGS84', DATUM['WGS84', SPHEROID['WGS84', 6378137.0, 298.257223563]], PRIMEM['Greenwich', 0.0], UNIT['degree', 0.017453292519943295], AXIS['Longitude',EAST], AXIS['Latitude',NORTH]]")
        presence.append( coord )
        p1 = Element('Point')
        p1.set('Id', '3')
        p1.set('X', '-72.83')
        p1.set('Y', '-12.33')
        presence.append( p1 )
        p2 = Element('Point')
        p2.set('Id', '4')
        p2.set('X', '-69.62')
        p2.set('Y', '-13.37')
        presence.append( p2 )
        sampler.append( presence )
        test_params.append( sampler )
        # Algorithm parameter
        test_params.append( serialized_model )
        # Statistics parameters
        stat_param = Element('Statistics')
        conf_matrix = Element('ConfusionMatrix')
        conf_matrix.set('Threshold', 0.5)
        stat_param.append( conf_matrix )
        test_params.append( stat_param )

        retries = 3
        while retries:
            try:
                ticket = soap_client.service.testModel( test_params )
                if ticket is None:
                    print 'No ticket returned in testModel!'
                    exit(22)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(f)
                        exit(23)
                else:
                    print 'Web fault:',str(f)
                    exit(24)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(u)
                        exit(25)
                else:
                    print 'URL error:',str(f)
                    exit(26)
            except Exception, e:
                print 'testModel call failure:',str(e)
                exit(27)

        print 'TestModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    print 'Model testing aborted!'
                    exit(27)
                now = time.time()
                if now - start > 20*60:
                    print 'Service is taking too long to test model. Aborting.'
                    exit(28)
                time.sleep(5)
                progress = check_status( soap_client, ticket )
        except Exception, e:
            print 'getProgress call failure:',str(e)
            exit(29)

        print 'GetProgress: OK'

        #####  GET TEST RESULT
        ###################################
        try:
            test_result = soap_client.service.getTestResult( ticket )
        except Exception, e:
            print 'getTestResult call failure:',str(e)
            exit(30)

        if not hasattr( test_result, 'Statistics' ):
            print 'Missing statistics!'
            exit(31)

        if not hasattr( test_result.Statistics, 'ConfusionMatrix' ):
            print 'Missing confusion matrix!'
            exit(32)

        print 'GetTestResult: OK'

    #####  PROJECT MODEL
    ###################################
    if run_projection:
        proj_params = Element('ProjectionParameters')
        proj_params.applyns( (None, xml_ns) )
        # Algorithm parameter
        proj_params.append( serialized_model )
        # Layers
        env = Element('Environment')
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        proj_params.append( env )
        # Output parameters
        output_params = Element('OutputParameters')
        output_params.set('FileType', 'ByteHFA' )
        tmpl_element = Element('TemplateLayer')
        tmpl_element.set('Id', layer1)
        output_params.append( tmpl_element )
        proj_params.append( output_params )
        # Statistics parameters
        stat_params = Element('Statistics')
        area_params = Element('AreaStatistics')
        area_params.set('PredictionThreshold', 0.5)
        stat_params.append( area_params )
        proj_params.append( stat_params )

        retries = 3
        while retries:
            try:
                ticket = soap_client.service.projectModel( proj_params )
                if ticket is None:
                    print 'No ticket returned in projectModel!'
                    exit(33)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(f)
                        exit(34)
                else:
                    print 'Web fault:',str(f)
                    exit(35)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(u)
                        exit(36)
                else:
                    print 'URL error:',str(f)
                    exit(37)
            except Exception, e:
                print 'projectModel call failure:',str(e)
                exit(38)

        print 'ProjectModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    print 'Model projection aborted!'
                    exit(39)
                now = time.time()
                if now - start > 20*60:
                    print 'Service is taking too long to project model. Aborting.'
                    exit(40)
                time.sleep(5)
                progress = check_status( soap_client, ticket )
        except Exception, e:
            print 'getProgress call failure:',str(e)
            exit(41)

        print 'GetProgress: OK'

        #####  GET PROJECTION METADATA
        ###################################
        try:
            statistics = soap_client.service.getProjectionMetadata( ticket )
        except Exception, e:
            print 'getProjectionMetadata call failure:',str(e)
            exit(42)

        if not hasattr( statistics, 'ProjectionEnvelope' ):
            print 'Missing projection envelope!'
            exit(43)

        if not hasattr( statistics.ProjectionEnvelope, 'AreaStatistics' ):
            print 'Missing area statistics!'
            exit(44)

        print 'Total cells:',statistics.ProjectionEnvelope.AreaStatistics._TotalCells
        print 'Suitable cells:',statistics.ProjectionEnvelope.AreaStatistics._CellsPredicted

        print 'GetProjectionMetadata: OK'

        #####  GET LAYER AS URL
        ###################################
        try:
            url = soap_client.service.getLayerAsUrl( ticket )
        except Exception, e:
            print 'getLayerAsUrl call failure:',str(e)
            exit(45)

        print 'Projection URL',url

        print 'GetLayerAsUrl: OK'

        try:
            info = urllib.urlopen( url )
            if info.code != 200:
                print 'Remote resource does not exist! ( HTTP status code',info.code,')'
                exit(46)
        except Exception, e:
            print 'URL unreachable:',str(e)
            exit(47)

    #####  EVALUATE MODEL
    ###################################
    if run_model_evaluation:
        eval_params = Element('ModelEvaluationParameters')
        eval_params.applyns( (None, xml_ns) )
        sampler = Element('Sampler')
        env = Element('Environment')
        # Layers
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        # Mask
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        sampler.append( env )
        # Points
        presence = Element('Presence')
        presence.set('Label', 'test_service.py')
        coord = Element('CoordinateSystem')
        coord.setText("GEOGCS['WGS84', DATUM['WGS84', SPHEROID['WGS84', 6378137.0, 298.257223563]], PRIMEM['Greenwich', 0.0], UNIT['degree', 0.017453292519943295], AXIS['Longitude',EAST], AXIS['Latitude',NORTH]]")
        presence.append( coord )
        p1 = Element('Point')
        p1.set('Id', '3')
        p1.set('X', '-72.83')
        p1.set('Y', '-12.33')
        presence.append( p1 )
        p2 = Element('Point')
        p2.set('Id', '4')
        p2.set('X', '-69.62')
        p2.set('Y', '-13.37')
        presence.append( p2 )
        sampler.append( presence )
        eval_params.append( sampler )
        # Algorithm parameter
        eval_params.append( serialized_model )

        retries = 3
        while retries:
            try:
                ticket = soap_client.service.evaluateModel( eval_params )
                if ticket is None:
                    print 'No ticket returned in evaluateModel!'
                    exit(48)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(f)
                        exit(49)
                else:
                    print 'Web fault:',str(f)
                    exit(50)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(u)
                        exit(51)
                else:
                    print 'URL error:',str(f)
                    exit(52)
            except Exception, e:
                print 'evaluateModel call failure:',str(e)
                exit(53)

        print 'EvaluateModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    print 'Model evaluation aborted!'
                    exit(54)
                now = time.time()
                if now - start > 20*60:
                    print 'Service is taking too long to test model. Aborting.'
                    exit(55)
                time.sleep(5)
                progress = check_status( soap_client, ticket )
        except Exception, e:
            print 'getProgress call failure:',str(e)
            exit(56)

        print 'GetProgress: OK'

        #####  GET MODEL EVALUATION
        ###################################
        try:
            eval_result = soap_client.service.getModelEvaluation( ticket )
        except Exception, e:
            print 'getModelEvaluation call failure:',str(e)
            exit(57)

        if not hasattr( eval_result, 'Values' ):
            print 'Missing values container!'
            exit(58)

        if not hasattr( eval_result.Values, '_V' ):
            print 'Missing values!'
            exit(59)

        print 'GetModelEvaluation: OK'

    #####  SAMPLE POINTS
    ###################################
    if run_sample_points:
        sample_params = Element('SamplingParameters')
        sample_params.applyns( (None, xml_ns) )
        env = Element('Environment')
        # Layers
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        # Mask
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        sample_params.append( env )
        # Options
        options = Element('Options')
        options.set('NumPoints', '2')
        occ_filter = Element('OccurrencesFilter')
        env_unique = Element('EnvironmentallyUnique')
        occ_filter.append( env_unique )
        options.append( occ_filter )
        sample_params.append( options )

        retries = 3
        while retries:
            try:
                ticket = soap_client.service.samplePoints( sample_params )
                if ticket is None:
                    print 'No ticket returned in samplePoints!'
                    exit(60)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(f)
                        exit(61)
                else:
                    print 'Web fault:',str(f)
                    exit(62)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        print 'Exceeded number of retries. Last fault:',str(u)
                        exit(63)
                else:
                    print 'URL error:',str(f)
                    exit(64)
            except Exception, e:
                print 'samplePoints call failure:',str(e)
                exit(65)

        print 'SamplePoints: OK','( ticket',ticket,')'

        # Keep running until job is finished
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    print 'Sample points aborted!'
                    exit(66)
                now = time.time()
                if now - start > 20*60:
                    print 'Service is taking too long to test model. Aborting.'
                    exit(67)
                time.sleep(5)
                progress = check_status( soap_client, ticket )
        except Exception, e:
            print 'getProgress call failure:',str(e)
            exit(68)

        print 'GetProgress: OK'

        #####  GET SAMPLING RESULT
        ###################################
        try:
            sampling_result = soap_client.service.getSamplingResult( ticket )
        except Exception, e:
            print 'getSamplingResult call failure:',str(e)
            exit(69)

        if not hasattr( sampling_result, 'Environment' ):
            print 'Missing Environment!'
            exit(70)

        if not hasattr( sampling_result.Environment, 'Map' ):
            print 'Missing Map!'
            exit(71)

        if not hasattr( sampling_result.Environment, 'Mask' ):
            print 'Missing Mask!'
            exit(72)

        if not hasattr( sampling_result, 'Absence' ):
            print 'Missing Absence!'
            exit(73)

        if not hasattr( sampling_result.Absence, 'CoordinateSystem' ):
            print 'Missing CoordinateSystem!'
            exit(74)

        if not hasattr( sampling_result.Absence, 'Point' ):
            print 'Missing Point!'
            exit(75)

        print 'GetSamplingResult: OK'

    print 'Finished test in',str(int(time.time()-started)),'seconds'

except Exception, e:
    print 'Caught exception:',str(e)
    exit(76)

exit(0)
