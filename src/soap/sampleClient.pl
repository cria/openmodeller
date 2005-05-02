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
my $uri = 'http://openmodeller.sf.net/ns/1.0';

### Possible command line parameters

use vars qw($opt_help, $opt_debug, $opt_server, $opt_proxy);

&Getopt::Long::GetOptions('help', 'debug', 'server=s', 'proxy=s');

print_help() if $opt_help;

print "\nWelcome to a demo openModeller SOAP client!\n";

print<<EOM;

Please notice that this interface is just an initial prototype
and mostly an example of how to build a SOAP client
to access an openModeller server. Many variables
(occurrences/absences points and environmental layers)
are still hardcoded...
EOM

### Some global variables

my $url = $opt_server || '';

my $proxy_firewall = $opt_proxy if (defined($opt_proxy));

my %algorithms;

my $soap = 0; # future soap object

### Interact with user

my %options = ( 0 => 'Ping service', 
		1 => 'Show available algorithms', 
		2 => 'Create model', 
		3 => 'Get distribution map', 
		4 => 'Exit' );

my $option = -1;

my $exit_option = scalar(keys %options);

while ($option != $exit_option or not exists($options{$option-1}))
{
    $option = get_option();

    if ($option == 1)
    {
	$option = (ping()) ? -1 : $exit_option;
    }
    if ($option == 2)
    {
	$option = (get_algorithms()) ? -1 : $exit_option;
    }
    elsif ($option == 3)
    {
	$option = (create_model()) ? -1 : $exit_option;
    }
    elsif ($option == 4)
    {
	$option = (get_distribution_map()) ? -1 : $exit_option;
    }
    elsif (!$option)
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
	  " -server=address  SOAP server address (URL)\n".
	  "  -proxy=address  Proxy firewall address (URL)\n\n";
    exit;
}

###########################################
#  Get user option from console interface # 
###########################################
sub get_option
{
    print "\nPlease select from one the options:\n\n";

    foreach my $key (sort(keys %options))
    {
	my $val = $key+1;
	print "  [$val] $options{$key}\n";
    }

    print "\nYour choice: ";

    my $choice = <STDIN>;
    chomp($choice);

    return int($choice);
}


########################
#  Get SOAP server URL # 
########################
sub get_url
{
    while (!$url)
    {
	print "\nWhere is the SOAP server located? (url address): ";
	
	$url = <STDIN>;

	chomp($url);
    }

    return $url;
}

###############################
#  Prepare SOAP client object # 
###############################
sub prepare_soap
{
    unless($soap)
    {
	my $server = get_url();

	unless ($opt_debug)
	{
	    $soap = SOAP::Lite
               #-> service('http://openmodeller.sf.net/ns/1.0/openmodeller.wsdl')
		-> uri($uri)
		-> proxy($server)
		-> encoding('iso-8859-1');
	}
	else
	{
	    $soap = SOAP::Lite
               #-> service('http://openmodeller.sf.net/ns/1.0/openmodeller.wsdl')
		-> uri($uri)
		-> proxy($server)
		-> encoding('iso-8859-1')
		-> on_debug(sub{print @_});
	}

	$soap->transport->proxy(http => $proxy_firewall) if defined($proxy_firewall);
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
	-> name('ping')
	-> prefix('om')
	-> uri($uri);

    my $response = $soap->call($method);
    
    unless ($response->fault or !$response->result)
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
    if (scalar(keys %algorithms) > 0)
    {
	%algorithms = ();
    }

    print "Requesting algorithms...\n" if $option == 2;
    
    my $method = SOAP::Data
	-> name('getAlgorithms')
	-> prefix('om')
	-> uri($uri);

    my $response = $soap->call($method);
    
    unless ($response->fault)
    { 
	my @algorithms = $response->valueof('//algorithm');

	my $num_algs = scalar(@algorithms);
	
	if ($num_algs > 0)
	{
	    print "Available algorithms ($num_algs):\n" if $option == 2;
	    
	    my $i = 1;
	    
	    foreach my $alg (@algorithms)
	    {
		print '-' x 25 . "\n" if $option == 2;

		my $alg_path = "/Envelope/Body/[1]/[1]/[$i]";
		
		my %algorithm = %{$alg};

		$algorithm{'om-id'} = $response->dataof($alg_path)->attr->{'om-id'};

		my $accepts_categorical_data = ($algorithm{'accepts-categorical-maps'}) ? 'yes': 'no';
		my $accepts_absence_data = ($algorithm{'accepts-absence-points'}) ? 'yes': 'no';
		
		print <<EOM if $option == 2;
  $algorithm{name}
     * creator: $algorithm{creator}
     * bibliography: $algorithm{bibliography}
     * developer: $algorithm{developer} - $algorithm{contact}
     * overview: $algorithm{overview}
     * accepts categorical data: $accepts_categorical_data
     * accepts absence data: $accepts_absence_data
EOM

		$algorithm{parameters} = ();

		my @params = $response->dataof("$alg_path/parameter");
		
		my $num_params = scalar(@params);

                if ($num_params > 0)
                {
		    print "     * parameters ($num_params):\n" if $option == 2;

		    foreach my $par (@params)
		    {
			my %complete_par = %{$par};
			my %parameter = %{$complete_par{'_value'}[0]};

			$algorithm{parameters}{$complete_par{'_attr'}{'om-id'}} = \%parameter;
			
			my $min = ($parameter{'has-min'}) ? '['.$parameter{'min'}: '(oo';
			my $max = ($parameter{'has-max'}) ? $parameter{'max'}.']': 'oo)';
			my $domain = $min . ', ' . $max;
		    
			print <<EOM if $option == 2;
        $parameter{name} ($parameter{overview})
          - type: $parameter{'data-type'}
          - domain: $domain
          - typical value: $parameter{'typical-value'}
EOM
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

#########################################
#  Get algorithm from console interface # 
#########################################
sub get_algorithm
{
    print "\nChoose an algorithm:\n\n";

    foreach my $key (sort(keys %algorithms))
    {
	print "  [$key] $algorithms{$key}{name}\n";
    }

    print "\nYour choice: ";

    my $choice = <STDIN>;
    chomp($choice);

    return int($choice);
}

###################################################
#  Get algorithm parameter from console interface # 
###################################################
sub get_algorithm_parameter
{
    my ($param) = @_;

    my $min = ($param->{'has-min'}) ? '['.$param->{'min'}: '(oo';
    my $max = ($param->{'has-max'}) ? $param->{'max'}.']': 'oo)';
    my $domain = $min . ', ' . $max;
    
    print "\nParameter: $param->{name}";
    print " ($param->{description})" if $param->{description};
    print "\n domain: $domain";
    print "\n default: $param->{'typical-value'}" if $param->{'typical-value'};

    print "\n value: ";

    my $val = <STDIN>;
    chomp($val);

    $val = $param->{'typical-value'} unless length($val);

    return $val;
}

#####################################
#  Request model creation to server # 
#####################################
sub create_model
{
    prepare_soap();

    if (scalar(keys %algorithms) == 0 and not get_algorithms())
    {
	return 0;
    }

    my $method = SOAP::Data
	-> name('createModel')
	-> prefix('om')
	-> uri($uri);

    ### Algorithm

    my $alg_code = get_algorithm();

    if (!$alg_code or not exists($algorithms{$alg_code}))
    {
	return 0;
    }

    my $algorithm = SOAP::Data
	-> name('algorithm')
	-> prefix('om')
	-> type('om:Algorithm')
	-> attr({'om-id'=>$algorithms{$alg_code}{'om-id'}});

    if (scalar(keys(%{$algorithms{$alg_code}{parameters}})))
    {
	my @parameters = ();

	foreach my $param (keys(%{$algorithms{$alg_code}{parameters}}))
	{
	    my $value = get_algorithm_parameter(\%{$algorithms{$alg_code}{parameters}{$param}});

	    push(@parameters, {'om-id'=> $param, 'value'=> $value});
	}

	my @alg_tags =  map(SOAP::Data->type('struct')->name('parameter')->attr(\%{$_}), @parameters);

	$algorithm->set_value(\@alg_tags);
    }

    ### Maps

    my $base_dir = '../../../examples/layers/'; # Hard coded! remember it's a simple prototype!

    my @maps = ( {'location'=> $base_dir.'rain_coolest', 'categorical' => 0},
		 {'location'=> $base_dir.'rain_tot'    , 'categorical' => 0},
		 {'location'=> $base_dir.'temp_dryest' , 'categorical' => 0} );

    @maps = map(SOAP::Data->name('map')->attr(\%{$_}), @maps);

    my $maps = SOAP::Data
	-> name('maps')
	-> prefix('om')
	-> type('om:Maps');
    
    $maps->set_value(\@maps);

    ### Mask

    my $mask = SOAP::Data
	->name('mask')
	->type('struct')
	->attr({'location'=>$base_dir.'rain_coolest'});

    ### Points

    my $wkt = "GEOGCS['1924 ellipsoid',DATUM['Not_specified',SPHEROID['International 1924',6378388,297,AUTHORITY['EPSG','7022']],AUTHORITY['EPSG','6022']],PRIMEM['Greenwich',0,AUTHORITY['EPSG','8901']],UNIT['degree',0.0174532925199433,AUTHORITY['EPSG','9108']],AUTHORITY['EPSG','4022']]";
    
    my $coordsystem = SOAP::Data
	-> name('coordsystem')
	-> type('string')
	-> value($wkt);

    my @presencePoints = ( {'latitude' => -11.15, 'longitude' => -68.85},
			   {'latitude' => -14.32, 'longitude' => -67.38},
			   {'latitude' => -15.52, 'longitude' => -67.15},
			   {'latitude' => -16.73, 'longitude' => -65.12},
			   {'latitude' => -17.80, 'longitude' => -63.17} );

    @presencePoints = map(SOAP::Data->type('struct')->name('point')->attr(\%{$_}), @presencePoints);

    my $presences = SOAP::Data
	-> name('presences')
	-> prefix('om')
	-> type('om:PresencePoints');
    
    $presences->set_value(\@presencePoints);
    
    my @absencePoints = ( {'latitude' => -47.07, 'longitude' => -22.82},
			  {'latitude' => -49.75, 'longitude' => -12.70},
			  {'latitude' => -50.37, 'longitude' => -3.52},
			  {'latitude' => -45.44, 'longitude' => -14.23},
			  {'latitude' => -51.07, 'longitude' => -6.88} );

    @absencePoints = map(SOAP::Data->type('struct')->name('point')->attr(\%{$_}), @absencePoints);
    
    my $absences = SOAP::Data
	-> name('absences')
	-> prefix('om')
	-> type('om:AbsencePoints');

    $absences->set_value(\@absencePoints);

    my $points = SOAP::Data
	-> name('points')
	-> prefix('om')
	-> type('om:Points')
	-> value(\SOAP::Data->value($coordsystem, $presences, $absences));

    ### Output specification

    my $header = SOAP::Data
	-> name('header')
	-> type('string')
	-> value($base_dir.'rain_coolest');

    my $scale = SOAP::Data
	-> name('scale')
	-> type('int')
	-> value(240);

    my $format = SOAP::Data
	-> name('format')
	-> type('string')
	-> value('.tif');

    my $output = SOAP::Data
	-> name('output')
	-> prefix('om')
	-> type('om:Output')
	-> value(\SOAP::Data->value($header, $scale, $format));
    
    my @params = ($points, $maps, $mask, $algorithm, $output);

    print "Requesting model... ";
    
    my $response = $soap->call($method => @params);

    unless ($response->fault)
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
	-> name('getDistributionMap')
	-> prefix('om')
	-> uri($uri);

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


