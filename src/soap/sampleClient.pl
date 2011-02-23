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

Please notice that this interface is just a simple prototype
and mostly an example of how to build a SOAP client
to access an openModeller server. Many variables
(occurrences/absences points) are still hardcoded...
EOM

print "\nNote: this machine has SOAP::Lite version $SOAP::Lite::VERSION\n";

### Some global variables

my $url = $opt_server || '';

my %algorithms;

my %layers;

my $soap = 0; # future soap object

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
	  " -server=address  SOAP server address (URL)\n\n".
          "If you are behind a proxy firewall, set the following environment variables\n".
          "to their respective values: HTTP_proxy, HTTP_proxy_user, HTTP_proxy_pass\n\n";
    exit;
}

###########################################
#  Get user option from console interface # 
###########################################
sub get_option
{
    print "\nPlease select from one the options:\n\n";

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
	print "\nWhere is the SOAP server located? (url address): ";
	
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

        if ( $opt_debug )
	{
            use SOAP::Lite +trace => [ transport => \&debug_soap ];
        }

        $soap = SOAP::Lite
                          #-> service('http://openmodeller.sf.net/ns/1.0/openmodeller.wsdl')
		           -> uri( $omws_uri )
		           -> proxy( $server, options => {compress_threshold => 10000} )
		           #-> proxy( $server )
		           -> encoding( 'iso-8859-1' );
    }
}

#################################################
#  Print out the actual XML request or response # 
#################################################
sub debug_soap
{
    my ( $in ) = @_;

    if ( ref( $in ) eq 'HTTP::Request' || ref( $in ) eq 'HTTP::Response' )
    {
        print "\n" . ref( $in ) . "\n\n";
        print $in->content . "\n\n";

        #open( OUT, ">>client.debug" );
        #print OUT $in->content;
        #print OUT "\n";
        #close( OUT );
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

			my %complete_par = %{$par};
			my %parameter = %{$complete_par{'_value'}[0]};

			$parameter{'Id'} = $response->dataof($param_path)->attr->{'Id'};

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

    print "Requesting layers...\n" if $option == 3;
    
    my $method = SOAP::Data
	-> name( 'getLayers' )
	-> prefix( 'omws' )
        -> uri( $omws_uri );

    my $response = $soap->call( $method );
    
    unless ( $response->fault )
    { 
        my $i = 1;

        # just get the layers - ignore groups
        foreach my $layer ( $response->dataof('//Layer') ) 
        {
            print "[$i] " . $layer->attr->{'Id'} . "\n" if $option == 3;

            $layers{$i} = $layer->attr->{'Id'};

            ++$i;
        }

        # change this
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

    my @layer_codes = get_layers_from_user();

    if ( scalar( @layer_codes ) == 0 )
    {
	return 0;
    }

    my @maps = ();

    foreach my $code ( @layer_codes )
    {
        push( @maps, {'Id'=> $layers{$code}, 'IsCategorical'=> 0} );
    }

    @maps = map( SOAP::Data->name('Map')->attr(\%{$_}), @maps );

    ### Mask

    my $mask_code = get_mask_from_user();

    if ( not exists( $layers{$mask_code} ) )
    {
	return 0;
    }

    my $mask = SOAP::Data
	->name( 'Mask' )
	->type( 'struct' )
	->attr( {'Id'=>$layers{$mask_code}} );

    ## Environment

    my $env = SOAP::Data
	-> name( 'Environment' )
        -> value( \SOAP::Data->value( @maps, $mask ) );

    ### Points

    my $wkt = "<CoordinateSystem>GEOGCS['1924 ellipsoid',DATUM['Not_specified',SPHEROID['International 1924',6378388,297,AUTHORITY['EPSG','7022']],AUTHORITY['EPSG','6022']],PRIMEM['Greenwich',0,AUTHORITY['EPSG','8901']],UNIT['degree',0.0174532925199433,AUTHORITY['EPSG','9108']],AUTHORITY['EPSG','4022']]</CoordinateSystem>";

    # Encode coord system directly in XML to avoid automatic xsi:types for the content
    my $coordsystem = SOAP::Data
	-> type( 'xml' => $wkt );

    my @presencePoints = ( {'Id' => '1', 'Y' => -11.15, 'X' => -68.85},
			   {'Id' => '2', 'Y' => -14.32, 'X' => -67.38},
			   {'Id' => '3', 'Y' => -15.52, 'X' => -67.15},
			   {'Id' => '4', 'Y' => -16.73, 'X' => -65.12},
			   {'Id' => '5', 'Y' => -17.80, 'X' => -63.17} );

    @presencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @presencePoints );

    my $presences = SOAP::Data
	-> name('Presence')
	-> attr( {'Label'=>'Test species'} )
        -> value( \SOAP::Data->value( $coordsystem, 
                                      @presencePoints ) );
    
    my @absencePoints = ( {'Id' => '6' , 'Y' => -47.07, 'X' => -22.82},
			  {'Id' => '7' , 'Y' => -49.75, 'X' => -12.70},
			  {'Id' => '8' , 'Y' => -50.37, 'X' => -3.52},
			  {'Id' => '9' , 'Y' => -45.44, 'X' => -14.23},
			  {'Id' => '10', 'Y' => -51.07, 'X' => -6.88} );

    @absencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @absencePoints );
    
    my $absences = SOAP::Data
	-> name('Absence')
	-> attr( {'Label'=>'Test species'} )
        -> value( \SOAP::Data->value( $coordsystem, 
                                      @absencePoints ) );

#    my $sampler = SOAP::Data
#	-> name( 'Sampler' )
#        -> value( \SOAP::Data->value( $env, $presences, $absences ) );

    my $sampler = SOAP::Data
	-> name( 'Sampler' )
        -> value( \SOAP::Data->value( $env, $presences ) );

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

    my $ticket = get_ticket_from_user();

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
	print "Progress: ".$response->result ."%\n";
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

    my $ticket = get_ticket_from_user();

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

    my $ticket = get_ticket_from_user();

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting model... ";

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

    my $ticket = get_ticket_from_user();

    if ( ! $ticket )
    {
	return 0;
    }

    print "Requesting test result... ";

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

    # Hard coded for now
    my $xml = '<TestParameters xmlns="http://openmodeller.cria.org.br/xml/1.0">
  <Sampler>
    <Environment NumLayers="2">
      <Map Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest.tif" IsCategorical="0"/>
      <Map Id="/home/renato/projects/openmodeller/examples/layers/temp_avg.tif" IsCategorical="0"/>
      <Mask Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest.tif"/>
    </Environment>
    <Presence Label="Acacia aculeatissima">
      <CoordinateSystem>
         GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9108"]],AXIS["Lat",NORTH],AXIS["Long",EAST],AUTHORITY["EPSG","4326"]]
      </CoordinateSystem>
      <Point Id="1" X="-67.845739" Y="-11.327340" />
      <Point Id="2" X="-69.549969" Y="-12.350801" />
    </Presence>
    <Absence Label="Acacia aculeatissima">
      <CoordinateSystem>
         GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9108"]],AXIS["Lat",NORTH],AXIS["Long",EAST],AUTHORITY["EPSG","4326"]]
      </CoordinateSystem>
      <Point Id="3" X="-68.245959" Y="-12.060403" />
      <Point Id="4" X="-74.534959" Y="-15.340201" />
    </Absence>
  </Sampler>
  <Algorithm Id="BIOCLIM" Version="0.2">
    <Parameters>
      <Parameter Id="StandardDeviationCutoff" Value="0.9"/>
    </Parameters>
    <Model>      <Bioclim Mean="208.8333333333333 2446.728352864583" StdDev="93.20701153883221 95.8400666007785" Minimum="90 2285.860107421875" Maximum="305 2565.010009765625"/>
      </Model>
  </Algorithm>
</TestParameters>';

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

    # Hard coded for now
    my $xml = '<ProjectionParameters xmlns="http://openmodeller.cria.org.br/xml/1.0">
<Algorithm Id="Bioclim" Version="0.2">
<Parameters>
<Parameter Id="StandardDeviationCutoff" Value="0.7"/></Parameters>
<Model>
<Bioclim Mean="221.6 2375.984008789062" StdDev="93.9951700886806 184.4701277015361" Minimum="90 1911.670043945312" Maximum="413 2565.010009765625"/></Model></Algorithm>
<Environment NumLayers="2">
<Map Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest" IsCategorical="0" Min="0" Max="2137"/>
<Map Id="/home/renato/projects/openmodeller/examples/layers/temp_avg" IsCategorical="0" Min="-545.4199829101562" Max="3342.52001953125"/>
<Mask Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest"/></Environment>
<OutputParameters FileType="FloatingHFA">
<TemplateLayer Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest"/>
</OutputParameters>
</ProjectionParameters>';

    # Encode coord system directly in XML to avoid automatic xsi:types for the content
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

    my $ticket = get_ticket_from_user();

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

    my $ticket = get_ticket_from_user();

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

    my $ticket = get_ticket_from_user();

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
    print "\nChoose the layers you want to use for model creation:\n\n";

    foreach my $i ( sort { $a <=> $b } ( keys %layers ) )
    {
	print "  [$i] $layers{$i}\n";
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
sub get_mask_from_user
{
    print "\nChoose an input mask from the layers above: ";

    my $choice = <STDIN>;
    chomp( $choice );

    return $choice;
}

######################################
#  Get ticket from console interface # 
######################################
sub get_ticket_from_user
{
    print "\nTicket number: ";

    my $value = <STDIN>;
    chomp($value);

    return $value;
}



