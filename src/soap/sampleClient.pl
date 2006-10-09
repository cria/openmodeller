#!/usr/local/bin/perl

# Simple perl client to test the openModeller SOAP interface
# 
# @file
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

my %options = ( 0 => 'Ping service', 
		1 => 'Show available algorithms', 
		2 => 'Show available layers', 
		3 => 'Create model', 
		4 => 'Get distribution map', 
		5 => 'Exit' );

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
	$option = ( get_distribution_map() ) ? -1 : $exit_option;
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

    foreach my $key ( sort( keys %options ) )
    {
	my $val = $key+1;
	print "  [$val] $options{$key}\n";
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
		           -> proxy( $server )
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

        open( OUT, ">>client.debug" );
        print OUT $in->content;
        print OUT "\n";
        close( OUT );
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
	my @algorithms = $response->valueof( '//AlgorithmMetadata' );

	my $num_algs = scalar( @algorithms );
	
	if ( $num_algs > 0 )
	{
	    print "Available algorithms ($num_algs):\n" if $option == 2;
	    
	    my $i = 1;
	    
	    foreach my $alg ( @algorithms )
	    {
		print '-' x 25 . "\n" if $option == 2;

		my $alg_path = "/Envelope/Body/[1]/[1]/[$i]";
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

			my $min = (defined($response->dataof($range_path)->attr->{'Min'})) ? '['.$response->dataof($range_path)->attr->{'Min'}: '(oo';
			my $max = (defined($response->dataof($range_path)->attr->{'Max'})) ? $response->dataof($range_path)->attr->{'Max'}.']': 'oo)';
			my $domain = $min . ', ' . $max;

			$parameter{'Min'} = $response->dataof($range_path)->attr->{'Min'};
			$parameter{'Max'} = $response->dataof($range_path)->attr->{'Max'};

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
	->attr( {'Id'=>$layers{$mask_code}, 'IsCategorical'=>'0'} );

    ## Environment

    my $env = SOAP::Data
	-> name( 'Environment' )
        -> value( \SOAP::Data->value( @maps, $mask ) );

    ### Points

    my $wkt = "GEOGCS['1924 ellipsoid',DATUM['Not_specified',SPHEROID['International 1924',6378388,297,AUTHORITY['EPSG','7022']],AUTHORITY['EPSG','6022']],PRIMEM['Greenwich',0,AUTHORITY['EPSG','8901']],UNIT['degree',0.0174532925199433,AUTHORITY['EPSG','9108']],AUTHORITY['EPSG','4022']]";
    
    my $coordsystem = SOAP::Data
	-> name( 'CoordinateSystem' )
	-> value( \SOAP::Data->value( $wkt ) );

    my @presencePoints = ( {'Y' => -11.15, 'X' => -68.85},
			   {'Y' => -14.32, 'X' => -67.38},
			   {'Y' => -15.52, 'X' => -67.15},
			   {'Y' => -16.73, 'X' => -65.12},
			   {'Y' => -17.80, 'X' => -63.17} );

    @presencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @presencePoints );

#    my $presences = SOAP::Data
#	-> name('Presence')
#        -> value( \SOAP::Data->value( $coordsystem, @presencePoints ) );

    my $presences = SOAP::Data
	-> name('Presence')
        -> value( \SOAP::Data->value( @presencePoints ) );
    
    my @absencePoints = ( {'Y' => -47.07, 'X' => -22.82},
			  {'Y' => -49.75, 'X' => -12.70},
			  {'Y' => -50.37, 'X' => -3.52},
			  {'Y' => -45.44, 'X' => -14.23},
			  {'Y' => -51.07, 'X' => -6.88} );

    @absencePoints = map( SOAP::Data->name('Point')->attr(\%{$_}), @absencePoints );
    
    my $absences = SOAP::Data
	-> name('Absence')
        -> value( \SOAP::Data->value( $coordsystem, @absencePoints ) );

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

#########################################
#  Get algorithm from console interface # 
#########################################
sub get_algorithm_from_user
{
    print "\nChoose an algorithm:\n\n";

    foreach my $key (sort(keys %algorithms))
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

#########################
#  Get distribution map # 
#########################
sub get_distribution_map
{
    print "Sorry! At the moment SOAP::Lite doesn't know how to handle DIME attachments.\n";
    return 1;

    prepare_soap();

    print "Requesting map...\n";
    
    my $method = SOAP::Data
	-> name( 'getDistributionMap' )
	-> prefix( 'omws' )
	-> uri( $omws_uri );

    my $ticket = SOAP::Data
	-> name('ticket')
	-> type('string')
	-> value('image.jpg');

    my $response = $soap->call($method => $ticket);
    
    unless ($response->fault or !$response->result)
    { 
	print "Result: ".$response->result ."\n";
    }
    else
    {
	print "Ops, found some problems:\n";
	print join ', ', $response->faultcode, $response->faultstring; 
	print "\n";
	return 0;
    }
}


