#!/usr/bin/env python

#################################################################################
# This program was created for the BioVeL project to test an OMWS 2.0 endpoint.
#
# To run it you need Python >= 2.4 (but very likely < 3.0) and the suds library
# that can be downloaded from:
# https://fedorahosted.org/releases/s/u/suds/python-suds-0.4.tar.gz
#
# The test runs most service operations, relying on the existence of the BIOCLIM
# algorithm and the following two layers on the server side:
#
# Minimum temperature during the coldest month:
layer1 = '/layers/en/terrestrial/climate/global/worldclim/present/bioclim/10arc-minutes/f6e850a0-04d9-11e1-be50-0800200c9a66'
# Precipitation during the driest month:
layer2 = '/layers/en/terrestrial/climate/global/worldclim/present/bioclim/10arc-minutes/3ed05d93-04da-11e1-be50-0800200c9a66'
#
# Call script with -h to check available options.
#################################################################################

def close(msg, code):
    """
    Print message and exit program with the given code.
    Nagios documentation for exit codes:
    0 - OK       - The plugin was able to check the service and it appeared to be functioning properly.
    1 - Warning  - The plugin was able to check the service, but it appeared to be above some warning
                   threshold or did not appear to be working properly.
    2 - Critical - The plugin detected that either the service was not running or it was above some
                   critical threshold.
    3 - Unknown  - Invalid command line arguments were supplied to the plugin or low-level failures
                   internal to the plugin (such as unable to fork, or open a tcp socket) that prevent
                   it from performing the specified operation. Higher-level errors (such as name
                   resolution errors, socket timeouts, etc) are outside of the control of plugins and
                   should generally NOT be reported as UNKNOWN states.
    """
    print msg
    exit(code)

try:
    # Import modules
    try:
        import datetime, time, string, logging, urllib
        from urllib2 import URLError
        from optparse import OptionParser
        from xml.sax import SAXParseException
    except Exception, e:
        close(str(e), 3)

    # Special check for external suds module
    try:
        from suds.client import Client
        from suds import WebFault
        from suds.sax.element import Element
        from suds.plugin import MessagePlugin
    except ImportError:
        close('Missing Python suds module', 3)
    
    started = time.time()

    class MyOptionParser(OptionParser):
        "Custom OptionParser to handle errors"
        def error(self, msg):
            close('Invalid parameters. Try -h for available options.', 3)

    # Handle arguments
    parser = MyOptionParser()
    parser.add_option("-e", "--endpoint" , dest="endpoint" , type="string", help="Service endpoint")
    parser.add_option("-v", "--verbosity", dest="verbosity", type="int"   , help="Verbosity level: 0 (default), 1, 2 or 3.")

    (options, args) = parser.parse_args()

    if options.endpoint is None:
        # Try: 'http://modeller.cria.org.br/ws2/om'
        close('Missing "endpoint" parameter', 3)
        
    endpoint = options.endpoint

    verbosity = options.verbosity

    ##### Custom classes & functions #####
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

    def check_job_status( client, ticket ):
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

        ticket = call_and_track_progress(soap_client.service, 'createModel', mod_params)

    def call_operation( service, method, params ):
        "Call asynchronous service method and get the job ticket"
        func = getattr(service, method)
        retries = 3
        while retries:
            try:
                ticket = func(params)
                if ticket is None:
                    close('No ticket returned by '+method, 2)
                ticket = str(ticket)
                break
            except WebFault, f:
                if string.find( f.fault.faultstring, 'Zlib/gzip error' ) > -1:
                    retries -= 1
                    if retries == 0:
                        msg = 'Exceeded number of retries for '+method
                        if verbosity > 1:
                            msg += '. Last fault: '+str(f)
                        close(msg, 2)
                else:
                    msg = method+' web fault'
                    if verbosity > 1:
                        msg += ': '+str(f)
                    close(msg, 2)
            except URLError, u:
                if string.find( str(u), 'Name or service not known' ) > -1:
                    retries -= 1
                    if retries == 0:
                        msg = 'Exceeded number of retries for '+method
                        if verbosity > 1:
                            msg += '. Last fault:'+str(u)
                        close(msg, 2)
                else:
                    msg = method+' URL error'
                    if verbosity > 1:
                        msg += ': '+str(f)
                    close(msg, 2)
            except SAXParseException:
                close(method+' call failure: could not parse response (Internal Server Error?)', 2)
            except Exception, e:
                msg = method+' call failure'
                if verbosity > 1:
                     msg += ': '+str(e)
                close(msg, 2)
        return ticket

    def track_progress( client, method, ticket ):
        "Track job progress until completed or aborted"
        start = time.time()
        try:
            progress = -1
            while progress != 100:
                if progress == -2:
                    close(method+' aborted', 2)
                if progress == -3:
                    close(method+' cancelled', 2)
                if progress == -4:
                    close(method+' job reported as unknown', 2)
                now = time.time()
                if now - start > 20*60:
                    close(method+' is taking too long to finish (server overload?)', 2)
                time.sleep(5)
                progress = check_job_status( client, ticket )
        except Exception, e:
            msg = 'getProgress call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)
    
    ### Test initialization ###

    if verbosity == 3:
        print "OMWS 2.0 test (v2)"
        print "=================="

    xml_ns = 'http://openmodeller.cria.org.br/xml/2.0'

    # General settings
    if '?' in endpoint:
        wsdl = endpoint + '&wsdl'
    else:
        wsdl = endpoint + '?wsdl'

    # Instantiate SOAP client
    if verbosity == 3:
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
        close('ping call failure: could not parse response (Internal Server Error?)', 2)
    except Exception, e:
        msg = 'ping call failure'
        if verbosity > 1:
            msg += ': '+str(e)
        close(msg, 2)

    if status <> 1:
        close('Service unavailable', 2)

    # If you need to see the XML messages:
    #if verbosity == 3:
    #    print 'request:'
    #    print DumperPlugin.dreq
    #    print 'response:'
    #    print DumperPlugin.dresp

    if verbosity == 3:
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
        msg = 'getAlgorithms call failure'
        if verbosity > 1:
            msg += ': '+str(e)
        close(msg, 2)

    num_algs = len( algs.Algorithms.Algorithm )
    if num_algs == 0:
        close('No algorithms available', 2)
    if verbosity == 3:
        print 'Found',num_algs,'algorithms'
    found_bioclim = False
    for alg in algs.Algorithms.Algorithm:
        if alg._Id == 'BIOCLIM':
            found_bioclim = True
            break
    if not found_bioclim:
        close('BIOCLIM algorithm not available', 1)

    if verbosity == 3:
        print 'GetAlgorithms: OK'

    #####  GET LAYERS
    ###################################
    try:
        group = soap_client.service.getLayers()
    except Exception, e:
        msg = 'getLayers call failure'
        if verbosity > 1:
            msg += ': '+str(e)
        close(msg, 2)

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
        close('No layers available on server', 1)
    if verbosity == 3:
        print 'Found',num_layers,'layers'
    # TODO: Get one of the layers to be used by the test, instead of relying on
    #       two specific layers.
    if not_found_layer1:
        close('Layer 1 used by the test not found on server', 1)
    if not_found_layer2:
        close('Layer 2 used by the test not found on server', 1)

    if verbosity == 3:
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
        # TODO: call samplePoints to get two points inside the selected layer, instead
        #       of relying on hard coded points.       
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

        ticket = call_operation(soap_client.service, 'createModel', mod_params)

        if verbosity == 3:
            print 'CreateModel: OK','( ticket',ticket,')'

        #####  GET PROGRESS
        ###################################

        # Keep running until job is finished
        track_progress(soap_client, 'createModel', ticket)
        
        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET MODEL
        ###################################
        try:
            model = soap_client.service.getModel( ticket )
        except Exception, e:
            msg = 'getModel call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if not hasattr( model, 'SerializedModel' ):
            close('No SerializedModel element in getModel response', 2)

        serialized_model = get_suds_element( model.SerializedModel.Algorithm, 'Algorithm' )

        if verbosity == 3:
            print 'GetModel: OK'

        #####  GET LOG
        ###################################
        try:
            log = soap_client.service.getLog( ticket )
        except Exception, e:
            msg = 'getLog call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if verbosity == 3:
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

        ticket = call_operation(soap_client.service, 'testModel', test_params)

        if verbosity == 3:
            print 'TestModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'testModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET TEST RESULT
        ###################################
        try:
            test_result = soap_client.service.getTestResult( ticket )
        except Exception, e:
            msg = 'getTestResult call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if not hasattr( test_result, 'Statistics' ):
            close('No Statistics element in getTestResult response', 2)

        if not hasattr( test_result.Statistics, 'ConfusionMatrix' ):
            close('No ConfusionMatrix element in getTestResult response', 2)

        if verbosity == 3:
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

        ticket = call_operation(soap_client.service, 'projectModel', proj_params)

        if verbosity == 3:
            print 'ProjectModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'projectModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET PROJECTION METADATA
        ###################################
        try:
            statistics = soap_client.service.getProjectionMetadata( ticket )
        except Exception, e:
            msg = 'getProjectionMetadata call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if not hasattr( statistics, 'ProjectionEnvelope' ):
            close('No ProjectionEnvelope element in getProjectionMetadata response', 2)

        if not hasattr( statistics.ProjectionEnvelope, 'AreaStatistics' ):
            close('No AreaStatistics element in getProjectionMetadata response', 2)

        if verbosity == 3:
            print 'Total cells:',statistics.ProjectionEnvelope.AreaStatistics._TotalCells
            print 'Suitable cells:',statistics.ProjectionEnvelope.AreaStatistics._CellsPredicted
            print 'GetProjectionMetadata: OK'

        #####  GET LAYER AS URL
        ###################################
        try:
            url = soap_client.service.getLayerAsUrl( ticket )
        except Exception, e:
            msg = 'getLayerAsUrl call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if verbosity == 3:
            print 'Projection URL',url

        try:
            info = urllib.urlopen( url )
            if info.code != 200:
                close('Remote projection file does not exist (HTTP status code '+str(info.code)+')', 2)
        except Exception, e:
            msg = 'Projection URL unreachable'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if verbosity == 3:
            print 'GetLayerAsUrl: OK'
            
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

        ticket = call_operation(soap_client.service, 'evaluateModel', eval_params)

        if verbosity == 3:
            print 'EvaluateModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'evaluateModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET MODEL EVALUATION
        ###################################
        try:
            eval_result = soap_client.service.getModelEvaluation( ticket )
        except Exception, e:
            msg = 'getModelEvaluation call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if not hasattr( eval_result, 'Values' ):
            close('No Values container element in getModelEvaluation response', 2)

        if not hasattr( eval_result.Values, '_V' ):
            close('No V attribute in Values element of getModelEvaluation response', 2)

        if verbosity == 3:
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

        ticket = call_operation(soap_client.service, 'samplePoints', sample_params)

        if verbosity == 3:
            print 'SamplePoints: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'samplePoints', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET SAMPLING RESULT
        ###################################
        try:
            sampling_result = soap_client.service.getSamplingResult( ticket )
        except Exception, e:
            msg = 'getSamplingResult call failure'
            if verbosity > 1:
                msg += ': '+str(e)
            close(msg, 2)

        if not hasattr( sampling_result, 'Environment' ):
            close('No Environment element in getSamplingResult response', 2)

        if not hasattr( sampling_result.Environment, 'Map' ):
            close('No Environment/Map element in getSamplingResult response', 2)

        if not hasattr( sampling_result.Environment, 'Mask' ):
            close('No Environment/Mask element in getSamplingResult response', 2)

        if not hasattr( sampling_result, 'Absence' ):
            close('No Absence element in getSamplingResult response', 2)

        if not hasattr( sampling_result.Absence, 'CoordinateSystem' ):
            close('No Absence/CoordinateSystem element in getSamplingResult response', 2)

        if not hasattr( sampling_result.Absence, 'Point' ):
            close('No Absence/Point element in getSamplingResult response', 2)

        if verbosity == 3:
            print 'GetSamplingResult: OK'

    if verbosity == 3:
        print 'Finished test in',str(int(time.time()-started)),'seconds'

except Exception, e:
    close('Caught exception: '+str(e), 3)

close('OK', 0)
