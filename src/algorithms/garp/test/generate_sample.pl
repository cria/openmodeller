#!perl

sub SampleSetValues1()
{
    $ct = 0;

    # point value (presence/absence)
    for ($k = 0; $k < 2; $k++)
    {
	# env layer values, from -1 to 1 in intervals of 0.1
	for ($i = -1.0; $i <= 1.0; $i += 0.1)
	{
	    # repetition: 2 exact copies of each samples 
	    for ($j = 0; $j < 2; $j++)
	    {
		printf " { %+3.1f, %+8.4f, %+8.4f }, // %3d\n", $k, $i, $i, $ct;
		$ct++;
	    }
	}
    }
}

sub SampleSetValues2()
{
    $ct = 0;
 
    # point value (presence/absence)
    for ($k = 0; $k < 2; $k++)
    {
	# env layer values, from -1 to 1 in intervals of 0.1
	for ($i = -1.0; $i <= 1.0; $i += 0.1)
	{
	    # repetition: 2 exact copies of each samples 
	    for ($j = 0; $j < 2; $j++)
	    {
		# skip when point value is 0 and it is second copy
		if (!$j || $k)
		{
		    printf " { %+3.1f, %+8.4f, %+8.4f }, // %3d\n", $k, $i, $i, $ct;
		    $ct++;
		}
	    }
	}
    }
}


#SampleSetValues1();
SampleSetValues2();
