#include  <def_mni.h>
#include  <def_module.h>

private  void  usage(
    char  executable[] )
{
    (void) fprintf( stderr, "%s  volume_filename\n", executable );
    (void) fprintf( stderr, "   activity_filename|none   nx ny nz\n" );
    (void) fprintf( stderr, "   input_polygons output_polygons\n");
    (void) fprintf( stderr, "   model_weight model_filename|avg|none\n" );
    (void) fprintf( stderr, "   original_positions|none max_distance\n" );
    (void) fprintf( stderr, "   min_curvature max_curvature\n" );
    (void) fprintf( stderr, "   fract_step max_step\n" );
    (void) fprintf( stderr, "   max_search_distance degrees_continuity\n" );
    (void) fprintf( stderr, "   min_isovalue max_isovalue +/-/n\n" );
    (void) fprintf( stderr, "   gradient_threshold angle tolerance\n" );
    (void) fprintf( stderr, "   max_iterations  stop_threshold\n" );
}

int  main( argc, argv )
    int    argc;
    char   *argv[];
{
    Status            status;
    Real              start_time, end_time;
    char              *volume_filename, *activity_filename;
    char              *input_filename, *output_filename;
    char              *model_filename, *normal_direction, *original_filename;
    Real              min_isovalue, max_isovalue, gradient_threshold;
    Real              model_weight, min_curvature_offset, max_curvature_offset;
    Real              angle, tolerance, max_distance;
    int               nx, ny, nz;
    deform_struct     deform;
    FILE              *file;
    File_formats      file_format;
    int               n_objects;
    object_struct     **object_list;
    Volume            volume, tmp;
    polygons_struct   *polygons;
    static char       *dim_names[] = { MIxspace, MIyspace, MIzspace };

    initialize_argument_processing( argc, argv );

    initialize_deformation_parameters( &deform );

    if( !get_string_argument( "", &volume_filename ) ||
        !get_string_argument( "", &activity_filename ) ||
        !get_int_argument( 0, &nx ) ||
        !get_int_argument( 0, &ny ) ||
        !get_int_argument( 0, &nz ) ||
        !get_string_argument( "", &input_filename ) ||
        !get_string_argument( "", &output_filename ) ||
        !get_real_argument( 0.0, &model_weight ) ||
        !get_string_argument( "", &model_filename ) ||
        !get_string_argument( "", &original_filename ) ||
        !get_real_argument( 0.0, &max_distance ) ||
        !get_real_argument( 0.0, &min_curvature_offset ) ||
        !get_real_argument( 0.0, &max_curvature_offset ) ||
        !get_real_argument( 0.0, &deform.fractional_step ) ||
        !get_real_argument( 0.0, &deform.max_step ) ||
        !get_real_argument( 0.0, &deform.max_search_distance ) ||
        !get_int_argument( 0.0, &deform.degrees_continuity ) ||
        !get_real_argument( 0.0, &min_isovalue ) ||
        !get_real_argument( 0.0, &max_isovalue ) ||
        !get_string_argument( "", &normal_direction ) ||
        !get_real_argument( 0.0, &gradient_threshold ) ||
        !get_real_argument( 0.0, &angle ) ||
        !get_real_argument( 0.0, &tolerance ) ||
        !get_int_argument( 0, &deform.max_iterations ) ||
        !get_real_argument( 0.0, &deform.stop_threshold ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    set_boundary_definition( &deform.boundary_definition,
                             min_isovalue, max_isovalue,
                             gradient_threshold, angle, normal_direction[0],
                             tolerance );

    if( add_deformation_model( &deform.deformation_model,
                           -1, model_weight, model_filename,
                           min_curvature_offset, max_curvature_offset ) != OK )
    {
        return( 1 );
    }

    deform.deform_data.type = VOLUME_DATA;

    status = input_volume( volume_filename, dim_names, FALSE, &volume );

    if( strcmp( activity_filename, "none" ) != 0 )
    {
        status = open_file_with_default_suffix( activity_filename, "act",
                                        READ_FILE, BINARY_FORMAT, &file );

        if( status == OK )
            status = io_volume_auxiliary_bit( file, READ_FILE,
                                              volume, ACTIVE_BIT );

        status = close_file( file );
    }

    if( nx > 0 && ny > 0 && nz > 0 )
    {
        tmp = smooth_resample_volume( volume, nx, ny, nz );

        delete_volume( volume );

        volume = tmp;
    }

    deform.deform_data.volume = volume;

    if( status == OK )
    {
        status = input_graphics_file( input_filename, &file_format,
                                      &n_objects, &object_list );
    }

    if( status == OK &&
        (n_objects != 1 || object_list[0]->object_type != POLYGONS) )
    {
        (void) fprintf( stderr, "File must contain 1 polygons struct.\n" );
        status = ERROR;
    }

    if( status == OK )
        polygons = get_polygons_ptr( object_list[0] );

    if( status == OK && strcmp( original_filename, "none" ) != 0 )
    {
        status = input_original_positions( &deform.deformation_model,
                                           original_filename,
                                           max_distance,
                                           polygons->n_points );
    }

    if( status == OK )
    {
        start_time = current_cpu_seconds();

        deform_polygons( polygons, &deform );
    }

    if( status == OK )
        compute_polygon_normals( polygons );

    if( status == OK )
        end_time = current_cpu_seconds();

    if( status == OK )
        status = output_graphics_file( output_filename, BINARY_FORMAT,
                                       n_objects, object_list );
/*
    print_time( "Total cpu time: %g %s\n", end_time - start_time );
*/

    return( status != OK );
}
