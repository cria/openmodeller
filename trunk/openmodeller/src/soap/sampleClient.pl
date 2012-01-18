#!/usr/local/bin/perl

# Simple console interface written in Perl to interact with 
# a modelling service.
# 
# @author Renato De Giovanni (renato [at] cria [dot] org [dot] br)
# @date   2004-04-06
# $Id$
# 
# LICENSE INFORMATION 
# 
# Copyright(c) 2003 by CRIA -
# Centro de Referencia em Informacao Ambiental
#
# http://www.cria.org.br
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details:
# 
# http://www.gnu.org/copyleft/gpl.html

use SOAP::Lite 
    +trace => [ transport => \&c_transport ],
    on_fault => sub { my($soap, $res) = @_;
                      print "SOAP::Lite fault! Aborting...\n";
		      die ref $res ? $res->faultstring : $soap->transport->status, "\n";
                };
use Getopt::Long;
use Data::Dumper;

### Settings

# namespace to be used
my $omws_uri = 'http://openmodeller.cria.org.br/ws/1.0';
my $om_uri = 'http://openmodeller.cria.org.br/xml/1.0';

### Possible command line parameters

use vars qw($opt_help, $opt_debug, $opt_server);

&Getopt::Long::GetOptions( 'help', 'debug', 'server=s' );

print_help() if $opt_help;

print "\nWelcome to the openModeller SOAP client demo!\n";

print<<EOM;

Please notice that this interface is a simple prototype
and mostly an example of how to build a SOAP client
to interact with an openModeller web service.
EOM

print "\nNote: this machine has SOAP::Lite version $SOAP::Lite::VERSION\n";

### Some global variables

my $url = $opt_server || '';

my %algorithms;

my %layers;

my $soap = 0; # future soap object

my $last_xml_resp = '';

### Interact with user

my %options = ( 0  => 'Ping service', 
		1  => 'Show available algorithms', 
		2  => 'Show available layers', 
		3  => 'Create model', 
		4  => 'Get job progress', 
		5  => 'Get model', 
		6  => 'Get log', 
		7  => 'Test model', 
		8  => 'Get test result', 
		9  => 'Project model', 
		10 => 'Get projection metadata', 
		11 => 'Get map as attachment', 
		12 => 'Get map as URL', 
		13 => 'Quit' );

my $option = -1;

my $exit_option = scalar( keys %options );

while ( $option != $exit_option or not exists( $options{$option-1} ) )
{
    $option = get_option();

    if ( $option == 1 )
    {
	$option = ( ping() ) ? -1 : $exit_option;
    }
    if ($option == 2)
    {
	$option = ( get_algorithms() ) ? -1 : $exit_option;
    }
    if ($option == 3)
    {
	$option = ( get_layers() ) ? -1 : $exit_option;
    }
    elsif ($option == 4)
    {
	$option = ( create_model() ) ? -1 : $exit_option;
    }
    elsif ($option == 5)
    {
	$option = ( get_progress() ) ? -1 : $exit_option;
    }
    elsif ($option == 6)
    {
	$option = ( get_model() ) ? -1 : $exit_option;
    }
    elsif ($option == 7)
    {
	$option = ( get_log() ) ? -1 : $exit_option;
    }
    elsif ($option == 8)
    {
	$option = ( test_model() ) ? -1 : $exit_option;
    }
    elsif ($option == 9)
    {
	$option = ( get_test_result() ) ? -1 : $exit_option;
    }
    elsif ($option == 10)
    {
	$option = ( project_model() ) ? -1 : $exit_option;
    }
    elsif ($option == 11)
    {
	$option = ( get_projection_metadata() ) ? -1 : $exit_option;
    }
    elsif ($option == 12)
    {
	$option = ( get_layer_as_attachment() ) ? -1 : $exit_option;
    }
    elsif ($option == 13)
    {
	$option = ( get_layer_as_url() ) ? -1 : $exit_option;
    }
    elsif ( ! $option )
    {
	$option = $exit_option;
    }
}

print "\nBye!\n";

exit;

######################################################################

######################
#  Command line help # 
######################
sub print_help
{
    print "\nUsage: $0 [options]\n".
          "           -help  Print this usage message\n".
	  "          -debug  Output all SOAP messages\n".
	  " -server=address  Web service address (URL)\n\n".
          "If you are behind a proxy firewall, set the following environment variables\n".
          "to their respective values: HTTP_proxy, HTTP_proxy_user, HTTP_proxy_pass\n\n";
    exit;
}

###########################################
#  Get user option from console interface # 
###########################################
sub get_option
{
    print "\nPlease select one the options:\n\n";

    my $exit_option = scalar( keys %options );

    foreach my $key ( sort { $a <=> $b } ( keys %options ) )
    {
	my $val = $key+1;
        my $option = ( $key == $exit_option - 1 ) ? ' q ' : "$val";
	print "  [$option] $options{$key}\n";
    }

    print "\nYour choice: ";

    my $choice = <STDIN>;
    chomp( $choice );

    return int( $choice );
}


########################
#  Get SOAP server URL # 
########################
sub get_url
{
    while ( ! $url )
    {
	print "\nPlease specify the service address (URL): ";
	
	$url = <STDIN>;

	chomp( $url );
    }

    return $url;
}

###############################
#  Prepare SOAP client object # 
###############################
sub prepare_soap
{
    unless( $soap )
    {
        my $server = get_url();

        $soap = SOAP::Lite
                          #-> service('http://openmodeller.sf.net/ns/1.0/openmodeller.wsdl')
		           -> uri( $omws_uri )
		           -> proxy( $server, options => {compress_threshold => 10000} )
		           #-> proxy( $server )
		           -> encoding( 'iso-8859-1' );
    }
}

#############################
#  Custom transport handler # 
#############################
sub c_transport
{
    my ( $in ) = @_;

    my $trans = ref( $in );

    if ( $trans eq 'HTTP::Request' || $trans eq 'HTTP::Response' )
    {
        if ( $opt_debug )
        {
            print "\n" . $trans . "\n\n";
            print $in->content . "\n\n";
            #open( OUT, ">>client.debug" );
            #print OUT $in->content;
            #print OUT "\n";
            #close( OUT );
        }

        if ( $trans eq 'HTTP::Response' )
        {
            $last_xml_resp = $in->content;
        }
    } 
}


################
#  Ping server # 
################
sub ping
{
    prepare_soap();

    print "Checking service... ";
    
    my $ini = time();

    my $method = SOAP::Data
	-> name( 'ping' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    my $response = $soap->call( $method );
    
    unless ( $response->fault or ! $response->result )
    { 
	my $interval = time() - $ini;

	print "OK ($interval s)\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }
}


###############################
#  Get algorithms from server # 
###############################
sub get_algorithms
{
    prepare_soap();

    # reset algorithms hash
    if ( scalar( keys %algorithms ) > 0 )
    {
	%algorithms = ();
    }

    print "Requesting algorithms...\n" if $option == 2;
    
    my $method = SOAP::Data
	-> name( 'getAlgorithms' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    my $response = $soap->call( $method );
    
    unless ( $response->fault )
    { 
	my @algorithms = $response->valueof( '//Algorithm' );

	my $num_algs = scalar( @algorithms );
	
	if ( $num_algs > 0 )
	{
	    print "Available algorithms ($num_algs):\n" if $option == 2;
	    
	    my $i = 1;
	    
	    foreach my $alg ( @algorithms )
	    {
		print '-' x 25 . "\n" if $option == 2;

		my $alg_path = "/Envelope/Body/[1]/[1]/[1]/[$i]";
		my $alg_designer_path = "$alg_path/Designers/Designer";
		my $alg_developer_path = "$alg_path/Developers/Developer";
		my $alg_parameters_path = "$alg_path/Parameters";
		
		my %algorithm = %{$alg};

		$algorithm{'Id'} = $response->dataof($alg_path)->attr->{'Id'};
		$algorithm{'Version'} = $response->dataof($alg_path)->attr->{'Version'};

		my $accepts_categorical_data = ($response->dataof($alg_path)->attr->{'AcceptsCategoricalMaps'}) ? 'yes': 'no';
		my $accepts_absence_data = ($response->dataof($alg_path)->attr->{'RequiresAbsencePoints'}) ? 'yes': 'no';
		
		my $alg_creator = $response->dataof($alg_designer_path)->attr->{'Name'};
		my $alg_developer = $response->dataof($alg_developer_path)->attr->{'Name'};
		my $alg_developer_contact = $response->dataof($alg_developer_path)->attr->{'Contact'};

		print <<EOM if $option == 2;
  $algorithm{'Name'}
     * creator: $alg_creator
     * bibliography: $algorithm{'Bibliography'}
     * developer: $alg_developer - $alg_developer_contact
     * overview: $algorithm{'Overview'}
     * accepts categorical data: $accepts_categorical_data
     * accepts absence data: $accepts_absence_data
EOM

		my @params = $response->dataof( "$alg_parameters_path/Parameter" );
		
		my $num_params = scalar( @params );

                if ( $num_params > 0 )
                {
		    print "     * parameters ($num_params):\n" if $option == 2;

                    my $j = 1;

		    foreach my $par ( @params )
		    {
	                my $param_path = "$alg_parameters_path/[$j]";

                        my %parameter = ();

			$parameter{'Id'} = $response->dataof($param_path)->attr->{'Id'};
			$parameter{'Name'} = $response->valueof($param_path.'/Name');
			$parameter{'Type'} = $response->valueof($param_path.'/Type');
			$parameter{'Default'} = $response->valueof($param_path.'/Default');
			$parameter{'Overview'} = $response->valueof($param_path.'/Overview');

	                my $range_path = "$alg_parameters_path/[$j]/AcceptedRange";

			my $min = '(oo';
			my $max = 'oo)';

			if ( defined( $response->dataof( $range_path ) ) )
			{
			    $min = '['.$response->dataof($range_path)->attr->{'Min'} if defined( $response->dataof($range_path)->attr->{'Min'} );
			    $max = $response->dataof($range_path)->attr->{'Max'}.']' if defined( $response->dataof($range_path)->attr->{'Max'} );

			    $parameter{'Min'} = $response->dataof($range_path)->attr->{'Min'};
			    $parameter{'Max'} = $response->dataof($range_path)->attr->{'Max'};
			}

			my $domain = $min . ', ' . $max;

			$algorithm{parameters}{$parameter{'Id'}} = \%parameter;

			print <<EOM if $option == 2;
        $parameter{'Name'} ($parameter{'Overview'})
          - type: $parameter{'Type'}
          - domain: $domain
          - default value: $parameter{'Default'}
EOM

                        ++$j;
                    }
		}

		$algorithms{$i} = \%algorithm;

		++$i;
	    }
	    print '-' x 25 . "\n" if $option == 2;
	}
	else
	{
	    print "Sorry, it seems that no algorithms are available from the server\n";
	}

	return $num_algs;
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }
}

###########################
#  Get layers from server # 
###########################
sub get_layers
{
    prepare_soap();

    # reset layers hash
    if ( scalar( keys %layers ) > 0)
    {
	%layers = ();
    }

    my $display = ($option == 3) ? 1 : 0;

    print "Requesting layers...\n" if $display;
    
    my $method = SOAP::Data
	-> name( 'getLayers' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    my $response = $soap->call( $method );
    
    unless ( $response->fault )
    { 
        my $level = 0;
        my $seq = 0;
        fetch_branch( $response, '//AvailableLayers', $level, \$seq, $display);
	return 1;
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }
}

#####################################
#  Request model creation to server # 
#####################################
sub create_model
{
    prepare_soap();

    if ( scalar( keys %algorithms ) == 0 and not get_algorithms() )
    {
	return 0;
    }

    my $method = SOAP::Data
	-> name( 'createModel' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Algorithm

    my $alg_code = get_algorithm_from_user();

    if ( ! $alg_code or not exists( $algorithms{$alg_code} ) )
    {
	return 0;
    }

    my @alg_parameters = ();

    if ( scalar( keys( %{$algorithms{$alg_code}{parameters}} ) ) )
    {
	foreach my $param (keys(%{$algorithms{$alg_code}{parameters}}))
	{
	    my $value = get_algorithm_parameter_from_user( \%{$algorithms{$alg_code}{parameters}{$param}} );

	    push( @alg_parameters, {'Id'=> $param, 'Value'=> $value} );
	}

	@alg_parameters = map( SOAP::Data->name('Parameter')->attr(\%{$_}), @alg_parameters );
    }

    my $parameters = SOAP::Data
	-> name( 'Parameters' )
        -> value( \SOAP::Data->value( @alg_parameters ) );

    my $algorithm = SOAP::Data
	-> name( 'Algorithm' )
	-> attr( {'Id'=>$algorithms{$alg_code}{'Id'}, 'Version'=>$algorithms{$alg_code}{'Version'}} )
        -> value( \SOAP::Data->value( $parameters ) );

    ### Maps

    if ( scalar( keys %layers ) == 0 and not get_layers() )
    {
	return 0;
    }

    my @layer_codes = get_layers_from_user('Choose the layers you want to use for model creation');

    if ( scalar( @layer_codes ) == 0 )
    {
	return 0;
    }

    my @maps = ();

    foreach my $code ( @layer_codes )
    {
        push( @maps, {'Id'=> $layers{$code}{'id'}, 'IsCategorical'=> 0} );
    }

    @maps = map( SOAP::Data->name('Map')->attr(\%{$_}), @maps );

    ### Mask

    my $mask_code = get_layer_from_user('Choose an input mask from the layers above');

    if ( not exists( $layers{$mask_code} ) )
    {
	return 0;
    }

    my $mask = SOAP::Data
	->name( 'Mask' )
	->type( 'struct' )
	->attr( {'Id'=>$layers{$mask_code}{'id'}} );

    ## Environment

    my $env = SOAP::Data
	-> name( 'Environment' )
        -> value( \SOAP::Data->value( @maps, $mask ) );

    ### Points

    # Get points from file
    $got_file = 0;

    # Default label
    my $label = 'Test species';

    # Default SRS
    my $wkt = "<CoordinateSystem>GEOGCS['1924 ellipsoid',DATUM['Not_specified',SPHEROID['International 1924',6378388,297,AUTHORITY['EPSG','7022']],AUTHORITY['EPSG','6022']],PRIMEM['Greenwich',0,AUTHORITY['EPSG','8901']],UNIT['degree',0.0174532925199433,AUTHORITY['EPSG','9108']],AUTHORITY['EPSG','4022']]</CoordinateSystem>";

    # Encode coord system directly in XML to avoid automatic xsi:types for the content
    my $coordsystem = SOAP::Data
	-> type( 'xml' => $wkt );

    # Data structures
    my @presencePoints = ();
    my @absencePoints = ();

    while ( ! $got_file )
    {
        my $points_file = get_string_from_user('File with points');

        unless ( length( $points_file ) )
        {
            print "No file specified. Aborting.\n";
	    return 0;
        }

        $points = read_file( $points_file );

        next unless ( $points );

        # Extract points
        for ( split /^/, $points )
        {
            my @point = split /\t/, $_;

            next if ( scalar( @point ) < 4 );

            if ( scalar( @point ) > 4 && $point[4] == 0 )
            {
                push( @absencePoints, {'Id' => $point[0], 'X' => $point[2], 'Y' => $point[3]} );
            }
            else
            {
                push( @presencePoints, {'Id' => $point[0], 'X' => $point[2], 'Y' => $point[3]} );
            }
        }

        unless ( scalar( @presencePoints ) )
        {
            print "No presence points in file!\n";
            next;
        }

        $got_file = 1;
    }

    @presencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @presencePoints );

    my $presences = SOAP::Data
        -> name('Presence')
        -> attr( {'Label'=>'Test species'} )
        -> value( \SOAP::Data->value( $coordsystem, 
                                      @presencePoints ) );

    my $sampler = 0;

    if ( scalar( @absencePoints ) )
    {
        @absencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @absencePoints );
    
        my $absences = SOAP::Data
	    -> name('Absence')
	    -> attr( {'Label'=>'Test species'} )
            -> value( \SOAP::Data->value( $coordsystem, 
                                          @absencePoints ) );
        $sampler = SOAP::Data
            -> name( 'Sampler' )
            -> value( \SOAP::Data->value( $env, $presences, $absences ) );
    }
    else
    {
        $sampler = SOAP::Data
            -> name( 'Sampler' )
            -> value( \SOAP::Data->value( $env, $presences ) );
    }

    my $model_parameters = SOAP::Data
	-> name( 'ModelParameters' )
	-> attr( {'xmlns'=>$om_uri} )
        -> value( \SOAP::Data->value( $sampler, $algorithm ) );

    print "Requesting model creation... ";
    
    my $response = $soap->call( $method => $model_parameters );

    unless ( $response->fault )
    { 
	print "Your ticket is: ".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

#################
#  Get progress # 
#################
sub get_progress
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getProgress' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting job progress... ";

    my $soap_ticket = SOAP::Data
	-> name( 'ticket' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    {
        my $prog = $response->result;
        if ( $prog == -1 )
        {
            print "Queued on server.\n";
        }
        elsif ( $prog == -2 )
        {
            print "Aborted!\n";
        }
        else
        {
	    print "Progress: ".$prog ."%\n";
        }
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

############
#  Get log # 
############
sub get_log
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getLog' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting log... ";

    my $soap_ticket = SOAP::Data
	-> name( 'ticket' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    { 
	print "Log: \n\n".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

##############
#  Get model # 
##############
sub get_model
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getModel' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Retrieving model... ";

    my $soap_ticket = SOAP::Data
	-> name( 'ticket' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    { 
        my $tmp = $last_xml_resp;
        if ( $tmp =~ m/.*(<SerializedModel>.*<\/SerializedModel>).*/s )
        {
            my $file_name = "$ticket.mod";
            open FILE, ">", $file_name or die $!;
            print FILE $1;
            close FILE or die $!;

            print "\nOK! Model saved in $file_name\n";
        }
        else
        {
	    print "Ops, could not find serialized model in the response\n";
	    return 0;
        }
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}


####################
#  Get test result # 
####################
sub get_test_result
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getTestResult' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Retrieving test result... ";

    my $soap_ticket = SOAP::Data
	-> name( 'ticket' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    { 
	print "OK\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

###############
#  test model # 
###############
sub test_model
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'testModel' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    # Get model from file
    my @model_parts = get_model_from_user();
    
    return 0 unless scalar(@model_parts);

    # Get points from file
    my $got_file = 0;

    # Default SRS
    my $srs = '<CoordinateSystem>GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9108"]],AXIS["Lat",NORTH],AXIS["Long",EAST],AUTHORITY["EPSG","4326"]]</CoordinateSystem>';

    # Default label
    my $label = 'Test species';

    # XML to be built
    my $external_presence_points = '';
    my $external_absence_points = '';

    while ( ! $got_file )
    {
        my $points_file = get_string_from_user('File with points');

        unless ( length( $points_file ) )
        {
            print "No file specified. Aborting.\n";
	    return 0;
        }

        $points = read_file( $points_file );

        next unless ( $points );

        # Extract points
        for ( split /^/, $points )
        {
            my @point = split /\t/, $_;

            next if ( scalar( @point ) < 4 );

            my $str = '<Point Id="'.$point[0].'" X="'.$point[2].'" Y="'.$point[3].'" />';

            if ( scalar( @point ) > 4 && $point[4] == 0 )
            {
                $external_absence_points .= $str;
            }
            else
            {
                $external_presence_points .= $str;
            }
        }

        if ( length( $external_presence_points ) )
        {
            $external_presence_points = '<Presence Label="'.$label.'">'.$srs.$external_presence_points.'</Presence>';
        }
        else
        {
            unless ( length( $external_absence_points ) )
            {
                print "No points in file!\n";
                next;
            }
        }

        if ( length( $external_absence_points ) )
        {
            $external_absence_points = '<Absence Label="'.$label.'">'.$srs.$external_absence_points.'</Absence>';
        }

        $got_file = 1;
    }

    # Build XML request
    my $xml = '<TestParameters xmlns="http://openmodeller.cria.org.br/xml/1.0"><Sampler>'. $model_parts[0] . $model_parts[1] . $model_parts[2] .'</Sampler>' . $model_parts[3] . '</TestParameters>';

    # Encode coord system directly in XML to avoid automatic xsi:types for the content
    my $xml_parameters = SOAP::Data
	-> type( 'xml' => $xml );

    print "Requesting model test... ";
    
    my $response = $soap->call( $method => $xml_parameters );

    unless ( $response->fault )
    { 
	print "Your ticket is: ".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

##################
#  Project model # 
##################
sub project_model
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'projectModel' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    # Get model from file
    my @model_parts = get_model_from_user();
    
    return 0 unless scalar(@model_parts);

    my $format = get_format_from_user();

    unless ( $format )
    {
	return 0;
    }

    ### Maps

    if ( scalar( keys %layers ) == 0 and not get_layers() )
    {
	return 0;
    }

    my @layer_codes = get_layers_from_user('Choose the layers you want to use for model projection');

    if ( scalar( @layer_codes ) == 0 )
    {
	return 0;
    }

    my $environment = '<Environment NumLayers="'.scalar( @layer_codes ).'">';

    foreach my $code ( @layer_codes )
    {
        $environment .= '<Map Id="'.$layers{$code}{'id'}.'"/>';
    }

    ### Mask

    my $mask_code = get_layer_from_user('Choose an output mask from the layers above');

    if ( not exists( $layers{$mask_code} ) )
    {
	return 0;
    }

    $environment .= '<Mask Id="'.$layers{$mask_code}{'id'}.'"/></Environment>';

    ### Template

    my $template_code = get_layer_from_user('Choose an output template for resolution and SRS from the layers above');

    if ( not exists( $layers{$template_code} ) )
    {
	return 0;
    }

    # Build XML
    my $xml = '<ProjectionParameters xmlns="http://openmodeller.cria.org.br/xml/1.0">
'.$model_parts[3].$environment.'<OutputParameters FileType="'.$format.'"><TemplateLayer Id="'.$layers{$template_code}{'id'}.'"/></OutputParameters></ProjectionParameters>';

    my $xml_parameters = SOAP::Data
	-> type( 'xml' => $xml );

    print "Requesting model projection... ";
    
    my $response = $soap->call( $method => $xml_parameters );

    unless ( $response->fault )
    { 
	print "Your ticket is: ".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

############################
#  Get projection metadata # 
############################
sub get_projection_metadata
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getProjectionMetadata' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting projection data... ";

    my $soap_ticket = SOAP::Data
	-> name( 'ticket' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    { 
	print "OK\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

##########################
#  Get map as attachment # 
##########################
sub get_layer_as_attachment
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getLayerAsAttachment' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
	-> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting map...\n";

    my $soap_ticket = SOAP::Data
	-> name( 'id' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );
    
    unless ( $response->fault or ! $response->result )
    { 
	print "OK\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }
}

############
#  Get log # 
############
sub get_layer_as_url
{
    prepare_soap();

    my $method = SOAP::Data
	-> name( 'getLayerAsUrl' )
        -> encodingStyle( 'http://xml.apache.org/xml-soap/literalxml' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    ### Get ticket

    my $ticket = get_string_from_user('Ticket number');

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting map URL... ";

    my $soap_ticket = SOAP::Data
	-> name( 'id' )
	-> type( 'string' )
	-> value( $ticket );
    
    my $response = $soap->call( $method => $soap_ticket );

    unless ( $response->fault )
    { 
	print "URL: ".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }

    return 1;
}

#########################################
#  Get algorithm from console interface # 
#########################################
sub get_algorithm_from_user
{
    print "\nChoose an algorithm:\n\n";

    foreach my $key (sort { $a <=> $b } (keys %algorithms))
    {
	print "  [$key] $algorithms{$key}{Name}\n";
    }

    print "\nYour choice: ";

    my $choice = <STDIN>;
    chomp($choice);

    return int($choice);
}

###################################################
#  Get algorithm parameter from console interface # 
###################################################
sub get_algorithm_parameter_from_user
{
    my ($param) = @_;

    my $min = (defined($param->{'Min'})) ? '['.$param->{'Min'}: '(oo';
    my $max = (defined($param->{'Max'})) ? $param->{'Max'}.']': 'oo)';
    my $domain = $min . ', ' . $max;
    
    print "\nParameter: $param->{Name}";
    print " ($param->{Description})" if $param->{Description};
    print "\n domain: $domain";
    print "\n default: $param->{'Default'}" if $param->{'Default'};

    print "\n value: ";

    my $val = <STDIN>;
    chomp($val);

    $val = $param->{'Default'} unless length($val);

    return $val;
}

######################################
#  Get layers from console interface # 
######################################
sub get_layers_from_user
{
    my ($label) = @_;

    print "\n$label:\n\n";

    foreach my $i ( sort { $a <=> $b } ( keys %layers ) )
    {
        if ( $layers{$i}{'is_layer'} )
        {
            print ' ' x $layers{$i}{'level'} . ' ['.$i.'] ' . $layers{$i}{'label'} . "\n";
        }
        else
        {
            print '-' x $layers{$i}{'level'} . ' ' . $layers{$i}{'label'} . "\n";
        }
    }

    print "\nYour choice (comma separated list of values): ";

    my $choice = <STDIN>;
    chomp( $choice );

    @layers = split( /\s*,\s*/, $choice );

    return @layers;
}

##########################################
#  Get input mask from console interface # 
##########################################
sub get_layer_from_user
{
    my ($label) = @_;

    print "\n$label: ";

    my $choice = <STDIN>;
    chomp( $choice );

    return $choice;
}

######################################
#  Get ticket from console interface # 
######################################
sub get_string_from_user
{
    my ($label) = @_;

    print "\n$label: ";

    my $value = <STDIN>;
    chomp($value);

    return $value;
}

#############################################
#  Read the content of a file and return it # 
#############################################
sub read_file
{
    my ($path) = @_;

    my $content = '';

    unless ( -e $path )
    {
        print "File does not exist!\n";
    }
    else
    {
        unless ( open( FILE, $path ) )
        {
            print "Could not open file!\n";
        }
        else
        {
            while (<FILE>)
            {
                $content .= $_;
            }

            close( FILE );

            unless ( $content )
            {
                print "File is empty!";
            }
        }
    }

    return $content;
}

########################
#  Get model from file # 
########################
sub get_model_from_user
{
    my $got_file = 0;

    my @model_parts = ();
    my $model = '';
    my $native_environment = '';
    my $internal_presence_points = '';
    my $internal_absence_points = '';
    my $algorithm = '';

    while ( ! $got_file )
    {
        my $model_file = get_string_from_user('File with the model');

        unless ( length( $model_file ) )
        {
            print "No file specified. Aborting.\n";
	    return @model_parts;
        }

        $model = read_file( $model_file );

        next unless ( $model );

        unless ( $model =~ m/.*(<Environment\s.*<\/Environment>).*(<Presence\s.*<\/Presence>).*(<Absence\s.*<\/Absence>)?.*(<Algorithm\s.*<\/Algorithm>).*/s )
        {
            print "Could not find serialized model in file!\n";
            next;
        }

        push( @model_parts, $1 ); # native_environment
        push( @model_parts, $2 ); # internal_presence_points 
        push( @model_parts, $3 ); # internal_absence_points
        push( @model_parts, $4 ); # algorithm

        $got_file = 1;
    }

    return @model_parts;
}

################################
#  Get output format from user # 
################################
sub get_format_from_user
{
    print "\nChoose an output format:\n\n";

    my %formats = ( 1 => {'code'=>'GreyTiff',    'label'=>'Grey scale GeoTiff [0,255]'},
                    2 => {'code'=>'GreyTiff100', 'label'=>'Grey scale GeoTiff [0,100]'}, 
                    3 => {'code'=>'FloatingTiff','label'=>'Floating point GeoTiff'}, 
                    4 => {'code'=>'GreyBMP',     'label'=>'Grey scale BMP'}, 
                    5 => {'code'=>'FloatingHFA', 'label'=>'Floating point Erdas Imagine'}, 
                    6 => {'code'=>'ByteHFA',     'label'=>'Byte Erdas Imagine [0,100]'},
                    7 => {'code'=>'ByteASC',     'label'=>'Byte ARC/Info ASCII grid [0,100]'},
                    8 => {'code'=>'FloatingASC', 'label'=>'Floating point ARC/Info ASCII grid'} );

    foreach my $key (sort { $a <=> $b } (keys %formats) )
    {
	print "  [$key] $formats{$key}{'label'}\n";
    }

    print "\nYour choice: ";

    my $choice = <STDIN>;
    chomp($choice);

    return 0 unless ( exists( $formats{$choice} ) );

    return $formats{$choice}{'code'};
}

######################################
#  Fetch a branch from getLayers XML # 
######################################
sub fetch_branch
{
    my ($resp, $path, $level, $seq, $display) = @_;

    my $lev = $level + 1;

    my $j = 0;

    foreach my $node ( $resp->dataof( $path.'/*' ) )
    {
        $j++;

        my $name = $node->name();

        if ( $name eq 'LayersGroup' )
        {
            my $label = $resp->valueof( $path."/[$j]/Label" );
            my $key = $$seq.'.'.$lev;
            $layers{$key}{'is_layer'} = 0;
            $layers{$key}{'id'} = 0;
            $layers{$key}{'label'} = $label;
            $layers{$key}{'level'} = $lev;
            print '-' x $lev . ' ' . $label . "\n" if $display;
            fetch_branch( $resp, $path."/[$j]", $lev, $seq, $display );
        }
        elsif ( $name eq 'Layer' )
        {
            $$seq = $$seq + 1;
            my $label = $resp->valueof( $path."/[$j]/Label" );
            my $lid = $node->attr->{'Id'};
            $layers{$$seq}{'is_layer'} = 1;
            $layers{$$seq}{'id'} = $lid;
            $layers{$$seq}{'label'} = $label;
            $layers{$$seq}{'level'} = $lev;
            print ' ' x $lev . ' ['.$$seq.'] ' . $label . "\n" if $display;
        }
    }

    return 1;
}


