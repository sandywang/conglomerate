#include  <internal_volume_io.h>
#include  <bicpl.h>

private  void  usage(
    STRING   executable )
{
    STRING  usage_str = "\n\
Usage: %s sphere.obj sphere_flat.obj input.obj output_flat.obj output_fixed dist\n\
\n\
     .\n\n";

    print_error( usage_str, executable, executable );
}


int  main(
    int    argc,
    char   *argv[] )
{
    STRING               sphere_filename, sphere_flat_filename, input_filename;
    STRING               output_flat_filename, output_fixed_filename;
    int                  p, n_objects, best_index;
    int                  sphere_vertex, patch_vertex;
    int                  n_fixed, *fixed_indices;
    Real                 distance, dist, best_dist;
    File_formats         format;
    object_struct        **object_list;
    polygons_struct      *sphere, *sphere_flat, *patch;
    Point                *init_points;
    FILE                 *file;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &sphere_filename ) ||
        !get_string_argument( NULL, &sphere_flat_filename ) ||
        !get_string_argument( NULL, &input_filename ) ||
        !get_string_argument( NULL, &output_flat_filename ) ||
        !get_string_argument( NULL, &output_fixed_filename ) ||
        !get_real_argument( 0.0, &distance ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    if( input_graphics_file( sphere_filename, &format, &n_objects,
                             &object_list ) != OK || n_objects != 1 ||
        get_object_type(object_list[0]) != POLYGONS )
        return( 1 );

    sphere = get_polygons_ptr( object_list[0] );

    if( input_graphics_file( sphere_flat_filename, &format, &n_objects,
                             &object_list ) != OK || n_objects != 1 ||
        get_object_type(object_list[0]) != POLYGONS )
        return( 1 );

    sphere_flat = get_polygons_ptr( object_list[0] );

    if( input_graphics_file( input_filename, &format, &n_objects,
                             &object_list ) != OK || n_objects != 1 ||
        get_object_type(object_list[0]) != POLYGONS )
        return( 1 );

    patch = get_polygons_ptr( object_list[0] );

/*
    create_polygons_bintree( patch, ROUND( (Real) patch->n_items * 0.2 ) );
*/

    ALLOC( init_points, patch->n_points );
    for_less( patch_vertex, 0, patch->n_points )
        fill_Point( init_points[patch_vertex], 0.0, 0.0, 0.0 );

    n_fixed = 0;
    fixed_indices = NULL;
    for_less( sphere_vertex, 0, sphere->n_points )
    { 
        best_dist = 0.0;
        best_index = -1;

        for_less( patch_vertex, 0, patch->n_points )
        {
            dist = sq_distance_between_points( &sphere->points[sphere_vertex],
                                               &patch->points[patch_vertex] );

            if( best_index < 0 || dist < best_dist )
            {
                best_dist = dist;
                best_index = patch_vertex;
            }
        }

        if( best_dist < distance * distance )
        {
            ADD_ELEMENT_TO_ARRAY( fixed_indices, n_fixed, best_index,
                                  DEFAULT_CHUNK_SIZE );
            init_points[best_index] = sphere_flat->points[sphere_vertex];
        }
    }

    if( n_fixed < 3 )
    {
        print_error( "Number of fixed vertices is only: %d\n" );
        print_error( "Cannot continue.\n" );
        return( 1 );
    }

    if( open_file( output_fixed_filename, WRITE_FILE, ASCII_FORMAT, &file )
                           != OK )
        return( 1 );

    for_less( p, 0, n_fixed )
    {
        if( output_int( file, fixed_indices[p] ) != OK ||
            output_newline( file ) != OK )
        {
            print_error( "Error writing fixed file.\n" );
            return( 1 );
        }
    }

    (void) close_file( file );

    FREE( patch->points );
    patch->points = init_points;
    (void) output_graphics_file( output_flat_filename, format, 1, object_list );

    return( 0 );
}