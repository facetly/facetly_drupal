<?php

function facetly_admin_reindex_form( $form_state ) {
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Reindex' 
    );
    return $form;
}

function facetly_admin_reindex_form_submit( $form, &$form_state ) {
    module_load_include( 'inc', 'facetly', 'facetly.batch' );
    $facetly = facetly_api_init();
    $facetly->productTruncate();
    $node_type = $form_state['values']['node_type'];

    $function = 'facetly_batch';
    $batch = $function();
    batch_set( $batch );

    // Redirection takes place as usual.
    $form_state['redirect'] = 'admin/settings/facetly/reindex';
}

function facetly_admin_template_settings_form( $form_state ) {
    $facetly_template = variable_get( 'facetly_tpl', array ());
    $form['facetly_tpl'] = array(
        '#tree' => TRUE 
    );
    $form['facetly_tpl']['search'] = array(
        '#type' => 'textarea',
        '#title' => t( 'Search Template' ),
        '#default_value' => $facetly_template['search'],
        '#rows' => 20 
    );
    $form['facetly_tpl']['facet'] = array(
        '#type' => 'textarea',
        '#title' => t( 'Facet Template' ),
        '#default_value' => $facetly_template['facet'],
        '#rows' => 20 
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t( 'Save' ) 
    );
    return $form;
}

function facetly_admin_template_settings_form_validate( $form, &$form_state ) {
    $facetly = facetly_api_init();
    $values = $form_state['values'];

    try {
        $output = $facetly->templateUpdate( $values['facetly_tpl']['search'], $values['facetly_tpl']['facet'] );
    }
    catch ( Exception $e ) {
        $error = $e->getMessage();
    }

    if ( trim( $error ) != "Template Saved" ) {
        if ( strstr( $error, 'Search Template' ) && strstr( $error, 'Facet Template' ) ) {
            form_set_error( 'facetly_tpl', t( $error ) );
        } else if ( strstr( $error, 'Search Template' ) ) {
            form_set_error( 'facetly_tpl][search', t( $error ) );
        } else if ( strstr( $error, 'Facet Template' ) ) {
            form_set_error( 'facetly_tpl][facet', t( $error ) );
        }
    } else {
        drupal_set_message( t( "Template Saved." ) );
    }
}

function facetly_admin_template_settings_form_submit( $form, &$form_state ) {
    $facetly = facetly_api_init();
    $facetly_template = variable_get( 'facetly_tpl', array ());
    $values = $form_state['values'];
    variable_set( 'facetly_tpl', $values['facetly_tpl'] );
}

function facetly_admin_settings_form( $form_state ) {
    $facetly_extra = variable_get( 'facetly_extra', array ());
    $form['facetly_key'] = array(
        '#type' => 'textfield',
        '#title' => t( 'Consumer Key' ),
        '#default_value' => variable_get( 'facetly_key', '' ) 
    );
    $form['facetly_secret'] = array(
        '#type' => 'textfield',
        '#title' => t( 'Consumer Secret' ),
        '#default_value' => variable_get( 'facetly_secret', '' ) 
    );

    $form['facetly_server'] = array(
        '#type' => 'textfield',
        '#title' => t( 'Server Name' ),
        '#default_value' => variable_get( 'facetly_server', '' ) 
    );
    $form['facetly_extra'] = array(
        '#type' => 'fieldset',
        '#title' => t( 'Additional Settings' ),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE 
    );
    $form['facetly_extra']['facetly_limit'] = array(
        '#type' => 'textfield',
        '#title' => t( 'Search Limit Setting' ),
        '#default_value' => variable_get( 'facetly_limit', '10' ) 
    );
    $form['facetly_extra']['facetly_extra_vars'] = array(
        '#type' => 'textfield',
        '#title' => t( 'Additional Variable' ),
        '#default_value' => variable_get( 'facetly_extra_vars', '' ) 
    );

    return system_settings_form( $form );
}

function facetly_admin_field_settings_form( $form_state ) {
    $facetly = facetly_api_init();
    $fields = $facetly->fieldSelect();


    if ( empty( $fields ) ) {
        $form['markup'] = array(
            '#type' => 'markup',
            '#value' => t( 'Cannot connect to Facetly server. Please contact support.' ) 
        );
        return $form;
    }

    $field = $fields->field;

    $preset = module_invoke_all( 'imagecache_default_presets' );

    $facetly_field_key = variable_get( 'facetly_field_key', array ());

    $query_cck = db_query( "SELECT * FROM {content_node_field}" );
    while ( $cck = db_fetch_object( $query_cck ) ) {
        $imagefield = db_result( db_query( "SELECT widget_type FROM {content_node_field_instance} WHERE widget_module='imagefield' AND field_name='%s'", $cck->field_name ) );
        if ( $imagefield ) {
            $query_imagecache = db_query( "SELECT presetname FROM {imagecache_preset} ORDER BY presetname" );
            while ( $imagecache = db_fetch_object( $query_imagecache ) ) {
                $options[$cck->field_name . ':' . $imagecache->presetname] = 'CCK ' . $cck->field_name . ':' . $imagecache->presetname;
            }
            foreach ( $preset as $pkey => $pval ) {
                $options[$cck->field_name . ':' . $pkey] = 'CCK ' . $cck->field_name . ':' . $pval["presetname"];
            }
        } else {
            $options[$cck->field_name] = 'CCK ' . $cck->field_name;
        }
    }
    $query_vocabulary = db_query( "SELECT * FROM {vocabulary}" );
    while ( $vocabulary = db_fetch_object( $query_vocabulary ) ) {
        $options[$vocabulary->name] = 'Vocabulary ' . $vocabulary->name;
    }
    $options[''] = '-- Not selected --';
    $options['node_nid'] = 'Node nid';
    $options['node_title'] = 'Node title';
    $options['node_body'] = 'Node body';
    //	$options['category'] = 'category';
    $options['node_created'] = 'Node created';
    $options['node_url'] = 'Node url';
    $options['node_sell_price'] = 'Node price';
    $type_text = array();
    $type_numeric = array();
    asort( $options );

    $form['facetly_field_key'] = array();

    $form['facetly_field_key'] = array(
         '#tree' => TRUE 
    );

    $form['facetly_field_key']['id'] = array(
        '#type' => 'select',
        '#title' => t( '.id' ),
        '#disabled' => TRUE,
        '#options' => $options,
        '#default_value' => 'node_nid' 
    );

    foreach ( $field as $fieldkey => $fieldvalue ) {
        $fieldname = $fieldvalue->name;
        $fieldtype = $fieldvalue->type;
        $txtfieldname = '.field.' . $fieldname;
        if ( $fieldname == 'price' ) {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => ( ( empty( $facetly_field_key[$fieldname] ) ) ? 'node_sell_price' : $facetly_field_key[$fieldname] ) 
            );
        } else if ( $fieldname == 'body' ) {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => ( ( empty( $facetly_field_key[$fieldname] ) ) ? 'node_body' : $facetly_field_key[$fieldname] ) 
            );
        } else if ( $fieldname == 'title' ) {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => ( ( empty( $facetly_field_key[$fieldname] ) ) ? 'node_title' : $facetly_field_key[$fieldname] ) 
            );
        } else if ( $fieldname == 'created' ) {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => ( ( empty( $facetly_field_key[$fieldname] ) ) ? 'node_created' : $facetly_field_key[$fieldname] ) 
            );
        } else if ( $fieldname == 'url' ) {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => ( ( empty( $facetly_field_key[$fieldname] ) ) ? 'node_url' : $facetly_field_key[$fieldname] ) 
            );
        } else {
            $form['facetly_field_key'][$fieldname] = array(
                '#type' => 'select',
                '#title' => $txtfieldname,
                '#options' => $options,
                '#default_value' => $facetly_field_key[$fieldname] 
            );
        }
    }

    ksort( $form['facetly_field_key'] );

    $query_type = db_query( "SELECT name, type FROM {node_type} ORDER BY type ASC" );
    $check = array();
    while ( $get_type = db_fetch_object( $query_type ) ) {
        $check[$get_type->type] = t( $get_type->name );
    }
    $facetly_node_type = variable_get( 'facetly_node_type', array ());
    $form['facetly_node_type'] = array(
        '#type' => 'checkboxes',
        '#title' => t( 'Node Type' ),
        '#options' => $check,
        '#default_value' => $facetly_node_type 
    );

    return system_settings_form( $form );
}