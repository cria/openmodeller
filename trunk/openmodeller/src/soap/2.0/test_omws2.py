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

    def call_operation( service, method, params=None, dump_req=False, dump_resp=False ):
        "Call service operation and check for errors"
        func = getattr(service, method)
        retries = 3
        while retries:
            try:
                response = func(params)
                if type(response) is tuple and response[0] != 200:
                    close(method+' call failure (HTTP status code '+str(response[0])+')', 2)
                if response is None:
                    close('No response returned by '+method, 2)
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
                print DumperPlugin.dreq
                close(method+' call failure: could not parse response (premature end of script headers?)', 2)
            except Exception, e:
                msg = method+' call failure'
                if verbosity > 1:
                     msg += ': '+str(e)
                close(msg, 2)
            finally:
                if dump_req:
                    #f = open('./temp.xml', 'w')
                    #f.write(DumperPlugin.dreq)
                    #f.close()
                    print 'REQUEST:'
                    print DumperPlugin.dreq
                if dump_resp:
                    print 'RESPONSE:'
                    print DumperPlugin.dresp
        return response

    def check_job_status( client, ticket ):
        "Check job status"
        status = call_operation(client.service, 'getProgress', ticket)
        return int( status )

    def track_progress( client, method, ticket ):
        "Track job progress until completed or aborted"
        start = time.time()
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
                close(method+' is taking too long to finish (server overload?)', 1)
            time.sleep(5)
            progress = check_job_status( client, ticket )
    
    ### Test initialization ###

    if verbosity == 3:
        print "OMWS 2.0 test (v4)"
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

    try:
        soap_client = Client(wsdl, location=endpoint, prettyxml=True, plugins=[DumperPlugin()])
    except URLError, u:
        close('XML Schema referenced by WSDL not found (release suds cache when changing schema location)', 1)

    soap_client.add_prefix('om', xml_ns)
    soap_client.add_prefix('soap', 'http://schemas.xmlsoap.org/soap/envelope/')

    # Ignore logs
    h = NullLogHandler()
    logging.getLogger('suds.client').addHandler(h)
    logging.getLogger('suds.umx.typed').addHandler(h)
    logging.getLogger('suds.plugin').addHandler(h)

    # Test configuration
    check_layers = False
    run_model = False
    run_model_test = False
    run_projection = False
    run_model_evaluation = False
    run_sample_points = False
    run_experiment = True

    #####  PING
    ###################################
    status = call_operation(soap_client.service, 'ping')

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
    algs = call_operation(soap_client.service, 'getAlgorithms')

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
    group = call_operation(soap_client.service, 'getLayers')

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
    if check_layers:
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
        presence.set('Label', 'test_omws2.py')
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

        ticket = str( call_operation(soap_client.service, 'createModel', mod_params) )

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
        model = call_operation(soap_client.service, 'getModel', ticket)

        if not hasattr( model, 'SerializedModel' ):
            close('No SerializedModel element in getModel response', 2)

        if not hasattr( model, 'SerializedModel' ):
            close('No SerializedModel element in getModel response', 2)
        if not hasattr( model.SerializedModel, 'Algorithm' ):
            close('No Algorithm element in getModel response', 2)

        serialized_model = get_suds_element( model.SerializedModel.Algorithm, 'Algorithm' )

        if verbosity == 3:
            print 'GetModel: OK'

        #####  GET LOG
        ###################################
        log = call_operation(soap_client.service, 'getLog', ticket)

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
        presence.set('Label', 'test_omws2.py')
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

        ticket = str( call_operation(soap_client.service, 'testModel', test_params) )

        if verbosity == 3:
            print 'TestModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'testModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET TEST RESULT
        ###################################
        test_result = call_operation(soap_client.service, 'getTestResult', ticket)

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

        ticket = str( call_operation(soap_client.service, 'projectModel', proj_params) )

        if verbosity == 3:
            print 'ProjectModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'projectModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET PROJECTION METADATA
        ###################################
        statistics = call_operation(soap_client.service, 'getProjectionMetadata', ticket)

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
        url = call_operation(soap_client.service, 'getLayerAsUrl', ticket)

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
        presence.set('Label', 'test_omws2.py')
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

        ticket = str( call_operation(soap_client.service, 'evaluateModel', eval_params) )

        if verbosity == 3:
            print 'EvaluateModel: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'evaluateModel', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET MODEL EVALUATION
        ###################################
        eval_result = call_operation(soap_client.service, 'getModelEvaluation', ticket)

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

        ticket = str( call_operation(soap_client.service, 'samplePoints', sample_params) )

        if verbosity == 3:
            print 'SamplePoints: OK','( ticket',ticket,')'

        # Keep running until job is finished
        track_progress(soap_client, 'samplePoints', ticket)

        if verbosity == 3:
            print 'GetProgress: OK'

        #####  GET SAMPLING RESULT
        ###################################
        sampling_result = call_operation(soap_client.service, 'getSamplingResult', ticket)

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

    #####  RUN EXPERIMENT
    ###################################
    if run_experiment:
        mapped_jobs = {'job1':'', 'job2':'', 'job3':'', 'job4':''}
        exp_params = Element('ExperimentParameters')
        exp_params.applyns( (None, xml_ns) )
        ## Environment
        env = Element('Environment')
        env.set('id', 'environment1')
        # Layers
        for ref_layer in layers:
            map_element = Element('Map')
            map_element.set('Id', ref_layer)
            env.append( map_element )
        # Mask
        mask_element = Element('Mask')
        mask_element.set('Id', layer1)
        env.append( mask_element )
        exp_params.append( env )
        ## Points
        presence = Element('Presence')
        presence.set('Label', 'test_omws2.py')
        presence.set('id', 'presence1')
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
        exp_params.append( presence )
        ## Algorithm
        alg_settings = Element('AlgorithmSettings')
        alg_settings.set('id', 'algorithm1')
        alg = Element('Algorithm')
        alg.set('Id', 'ENVDIST')
        alg.set('Version', '0.5')
        params = Element('Parameters')
        param1 = Element('Parameter') 
        param1.set('Id', 'DistanceType')
        param1.set('Value', '1')
        param2 = Element('Parameter') 
        param2.set('Id', 'NearestPoints')
        param2.set('Value', '0')
        param3 = Element('Parameter') 
        param3.set('Id', 'MaxDistance')
        param3.set('Value', '0.1')
        params.append( param1 )
        params.append( param2 )
        params.append( param3 )
        alg.append( params )
        alg_settings.append( alg )
        exp_params.append( alg_settings )
        ## Jobs
        jobs = Element('Jobs')
        # sampling job
        sampling_job = Element('SamplingJob')
        sampling_job.set('id', 'job1')
        env_ref = Element('EnvironmentRef')
        env_ref.set('idref', 'environment1')
        options = Element('Options')
        options.set('NumPoints', '100')
        options.set('Label', 'Background')
        options.set('ProportionOfAbsences', '1.0')
        ofilter = Element('OccurrencesFilter')
        env_unique = Element('EnvironmentallyUnique')
        ofilter.append( env_unique )
        options.append( ofilter )
        sampling_job.append( env_ref )
        sampling_job.append( options )
        jobs.append( sampling_job )
        # create model job
        createmodel_job = Element('CreateModelJob')
        createmodel_job.set('id', 'job2')
        pres_ref = Element('PresenceRef')
        pres_ref.set('idref', 'presence1')
        alg_ref = Element('AlgorithmRef')
        alg_ref.set('idref', 'algorithm1')
        createmodel_job.append( env_ref )
        createmodel_job.append( pres_ref )
        createmodel_job.append( alg_ref )
        jobs.append( createmodel_job )
        # test model job
        testmodel_job = Element('TestModelJob')
        testmodel_job.set('id', 'job3')
        pres_ref = Element('PresenceRef')
        pres_ref.set('idref', 'presence1')
        abs_ref = Element('AbsenceRef')
        abs_ref.set('idref', 'job1')
        model_ref = Element('ModelRef')
        model_ref.set('idref', 'job2')
        testmodel_job.append( env_ref )
        testmodel_job.append( pres_ref )
        testmodel_job.append( abs_ref )
        testmodel_job.append( model_ref )
        stats = Element('Statistics')
        roc = Element('RocCurve')
        roc.set('BackgroundPoints', '100')
        roc.set('Resolution', '10')
        roc.set('MaxOmission', '1.0')
        roc.set('UseAbsencesAsBackground', '1')
        conf_matrix = Element('ConfusionMatrix')
        conf_matrix.set('Threshold', 'lpt')
        conf_matrix.set('IgnoreAbsences', '1')
        stats.append( conf_matrix )
        stats.append( roc )
        testmodel_job.append( stats )
        jobs.append( testmodel_job )
        # project model job
        projectmodel_job = Element('ProjectModelJob')
        projectmodel_job.set('id', 'job4')
        projectmodel_job.append( env_ref )
        projectmodel_job.append( model_ref )
        out_params = Element('OutputParameters')
        out_params.set('FileType', 'ByteHFA')
        template = Element('TemplateLayer')
        template.set('Id', layer1)
        out_params.append( template )
        projectmodel_job.append( out_params )
        jobs.append( projectmodel_job )
        # evaluate model job
        evalmodel_job = Element('EvaluateModelJob')
        evalmodel_job.set('id', 'job5')
        pres_ref = Element('PresenceRef')
        pres_ref.set('idref', 'presence1')
        model_ref = Element('ModelRef')
        model_ref.set('idref', 'job2')
        evalmodel_job.append( env_ref )
        evalmodel_job.append( pres_ref )
        evalmodel_job.append( model_ref )
        jobs.append( evalmodel_job )

        exp_params.append( jobs )

        exp_response = call_operation(soap_client.service, 'runExperiment', exp_params)

        tickets = []

        if not hasattr( exp_response, 'Job' ):
            close('No Jobs found in runExperiment response', 2)

        for job in exp_response.Job:
            tickets.append( job._Ticket )

        if len( tickets ) == 0:
            close('No tickets found in runExperiment response', 2)

        if verbosity == 3:
            print 'RunExperiment: OK','( tickets:',', '.join(tickets),')'

        # Keep running until all jobs are finished
        cnt = 0
        for ticket in tickets:
            cnt += 1
            mapped_jobs['job'+str(cnt)] = ticket
            track_progress(soap_client, 'runExperiment', ticket)
            if verbosity == 3:
                print 'Finished',ticket

        #####  GET RESULTS
        ###################################
        results = call_operation(soap_client.service, 'getResults', ','.join(tickets))

        if not hasattr( results, 'Job' ):
            close('No Jobs found in getResults response', 2)

        for job in results.Job:
            if not hasattr( job, '_Ticket' ):
                close('Missing ticket for job in getResults response', 2)
            ticket = job._Ticket
            if ticket not in mapped_jobs.values():
                close('Unknown ticket ('+ticket+') in getResults response', 2)
            if mapped_jobs['job1'] == ticket:
                # Sample points job
                if not hasattr( job, 'Sampler' ):
                    close('No Sampler element in sample points job response', 2)
                if not hasattr( job.Sampler, 'Environment' ):
                    close('No Environment element in sample points job response', 2)
                if not hasattr( job.Sampler.Environment, 'Map' ):
                    close('No Environment/Map element in sample points job response', 2)
                if not hasattr( job.Sampler.Environment, 'Mask' ):
                    close('No Environment/Mask element in sample points job response', 2)
                if not hasattr( job.Sampler, 'Absence' ):
                    close('No Absence element in sample points job response', 2)
                if not hasattr( job.Sampler.Absence, 'CoordinateSystem' ):
                    close('No Absence/CoordinateSystem element in sample points job response', 2)
                if not hasattr( job.Sampler.Absence, 'Point' ):
                    close('No Absence/Point element in sample points job response', 2)
            if mapped_jobs['job2'] == ticket:
                # Model creation job
                if not hasattr( job, 'ModelEnvelope' ):
                    close('No ModelEnvelope element in create model job response', 2)
                if not hasattr( job.ModelEnvelope, 'SerializedModel' ):
                    close('No SerializedModel element in create model job response', 2)
                if not hasattr( job.ModelEnvelope.SerializedModel, 'Algorithm' ):
                    close('No Algorithm element in create model job response', 2)
            if mapped_jobs['job3'] == ticket:
                # Model testing job
                if not hasattr( job, 'TestResultEnvelope' ):
                    close('No TestResultEnvelope element in test model job response', 2)
                if not hasattr( job.TestResultEnvelope, 'Statistics' ):
                    close('No Statistics element in test model job response', 2)
                if not hasattr( job.TestResultEnvelope.Statistics, 'ConfusionMatrix' ):
                    close('No ConfusionMatrix element in test model job response', 2)
                if not hasattr( job.TestResultEnvelope.Statistics.ConfusionMatrix, '_Threshold' ):
                    close('No Threshold element in test model job response', 2)
                threshold = float(job.TestResultEnvelope.Statistics.ConfusionMatrix._Threshold)
                if threshold < 0.640 or threshold > 0.6405:
                    # Exact value from openModeller is 0.64038
                    close('Incorrect LPT threshold in test model job response', 2)
            if mapped_jobs['job4'] == ticket:
                # Projection job
                if not hasattr( job, 'ProjectionEnvelope' ):
                    close('No ProjectionEnvelope element in project model job response', 2)
                if not hasattr( job.ProjectionEnvelope, '_url' ):
                    close('No url attribute in project model job response', 2)
                if not hasattr( job.ProjectionEnvelope, 'Statistics' ):
                    close('No Statistics element in project model job response', 2)
                if not hasattr( job.ProjectionEnvelope.Statistics, 'AreaStatistics' ):
                    close('No AreaStatistics element in project model job response', 2)
            if mapped_jobs['job5'] == ticket:
                # Model evaluation job
                if not hasattr( job, 'Values' ):
                    close('No Values container element in model evaluation job response', 2)
                if not hasattr( job.Values, '_V' ):
                    close('No V attribute in Values element of model evaluation job response', 2)

        if verbosity == 3:
            print 'GetResults: OK'

    if verbosity == 3:
        print 'Finished test in',str(int(time.time()-started)),'seconds'

except Exception, e:
    close('Caught exception: '+str(e), 3)

close('OK', 0)
