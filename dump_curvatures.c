#include  <internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int   argc,
    char  *argv[] )
{
    STRING               object_filename, output_filename;
    FILE                 *file;
    File_formats         format;
    int                  i, n_objects;
    object_struct        **objects;
    polygons_struct      *polygons;
    Real                 curvature_distance, *curvatures;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &object_filename ) ||
        !get_string_argument( "", &output_filename ) )
    {
        print( "Usage: %s  object_file  output_file  [curvature_distance]\n", argv[0] );
        return( 1 );
    }

    (void) get_real_argument( 4.0, &curvature_distance );

    if( input_graphics_file( object_filename,
                             &format, &n_objects, &objects ) != OK )
        return( 1 );

    if( n_objects != 1 || get_object_type( objects[0] ) != POLYGONS )
    {
        print( "File must contain 1 polygons object.\n" );
        return( 1 );
    }

    polygons = get_polygons_ptr( objects[0] );

    ALLOC( curvatures, polygons->n_points );

    get_polygon_vertex_curvatures( polygons, curvature_distance, 0.0,
                                   curvatures );

    if( open_file( output_filename, WRITE_FILE, ASCII_FORMAT, &file ) != OK )
        return( 1 );

    for_less( i, 0, polygons->n_points )
    {
        if( curvatures[i] < 1.0e30 )
        {
            if( output_real( file, curvatures[i] ) != OK ||
                output_newline( file ) != OK )
                break;
        }
    }

    (void) close_file( file );

    delete_object_list( n_objects, objects );

    return( 0 );
}