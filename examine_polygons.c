#include  <bicpl.h>

private  Real  get_poly_size(
    polygons_struct   *polygons,
    int               poly )
{
    int    size, i, point_index;
    Real   max_dist, dist;
    Point  centroid;
    Point  point;

    size = GET_OBJECT_SIZE( *polygons, poly );

    fill_Point( centroid, 0.0, 0.0, 0.0 );
    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                          POINT_INDEX( polygons->end_indices, poly, i )];
        point = polygons->points[point_index];
        ADD_POINTS( centroid, centroid, point );
    }

    SCALE_POINT( centroid, centroid, 1.0 / (Real) size );

    max_dist = 0.0;
    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                          POINT_INDEX( polygons->end_indices, poly, i )];
        point = polygons->points[point_index];

        dist = distance_between_points( &point, &centroid );

        if( i == 0 || dist > max_dist )
            max_dist = dist;
    }

    return( max_dist );
}

private  void  display_poly_info(
    polygons_struct   *polygons,
    int               poly )
{
    int    size, i, point_index;
    Real   max_dist, dist;
    Point  centroid;
    Point  point;

    size = GET_OBJECT_SIZE( *polygons, poly );

    fill_Point( centroid, 0.0, 0.0, 0.0 );
    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                          POINT_INDEX( polygons->end_indices, poly, i )];
        point = polygons->points[point_index];
        ADD_POINTS( centroid, centroid, point );
    }

    SCALE_POINT( centroid, centroid, 1.0 / (Real) size );

    max_dist = 0.0;
    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                          POINT_INDEX( polygons->end_indices, poly, i )];
        point = polygons->points[point_index];

        dist = distance_between_points( &point, &centroid );

        if( i == 0 || dist > max_dist )
            max_dist = dist;
    }

    print( "Max dist %g\n", dist );
}

int  main(
    int    argc,
    char   *argv[] )
{
    Status          status;
    char            *input_filename;
    int             n_objects, poly;
    File_formats    format;
    polygons_struct *polygons;
    object_struct   **object_list;

    status = OK;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_filename ) )
    {
        (void) fprintf( stderr, "Must have a filename argument.\n" );
        return( 1 );
    }

    status = input_graphics_file( input_filename, &format, &n_objects,
                                  &object_list );

    if( status == OK )
        print( "Objects input.\n" );

    if( status == OK && n_objects < 0 ||
        object_list[0]->object_type != POLYGONS )
        status = ERROR;

    if( status == OK )
    {
        int    i, start;
        Real   threshold, dist;

        polygons = get_polygons_ptr( object_list[0] );

        print( "Enter size threshold: " );
        while( input_real( stdin, &threshold ) == OK )
        {
            start = -1;
            for_less( i, 0, polygons->n_items+1 )
            {
                if( i < polygons->n_items )
                    dist = get_poly_size( polygons, i );
                if( i < polygons->n_items && dist > threshold )
                {
                    if( start == -1 )
                        start = i;
                }
                else if( start != -1 )
                {
                    if( start != i-1 )
                        print( " (%d,%d)-(%d,%d)", start / 256, start % 256, (i-1)/256, (i-1)%256 );
                    else
                        print( " (%d,%d)", (i-1)/256,(i-1)%256 );
                    start = -1;
                }
            }

            print( "\n\nEnter size threshold: " );
        }
/*
        print( "Enter polygon index: " );
        while( input_int( stdin, &poly ) == OK )
        {
            display_poly_info( polygons, poly );
            print( "Enter polygon index: " );
        }
*/
    }

    if( status == OK )
        delete_object_list( n_objects, object_list );

    if( status == OK )
        print( "Objects output.\n" );

    return( status != OK );
}
