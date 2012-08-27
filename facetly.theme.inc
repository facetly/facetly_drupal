<?php

function theme_facetly_item_list($items = array(), $title = NULL, $type = 'ul', $attributes = NULL) {
	  $output = '<div class="item-list">';
	  if (isset($title)) {
		$output .= '<h3>' . $title . '</h3>';
	  }

	  if (!empty($items)) {
		$output .= "<$type" . drupal_attributes($attributes) . '>';
		$num_items = count($items);
		$itemIterator = new ArrayIterator($items);
		foreach ($itemIterator as $i => &$item) {
		  $attributes = array();
		  $children = array();
		  if (is_array($item)) {
		  	$itemI = new ArrayIterator($item);
		    foreach ($itemI as $key => &$value) {
		      if ($key == 'data') {
		        $data = $value;
		      }
		      elseif ($key == 'children') {
		        $children = $value;
		      }
		      else {
		        $attributes[$key] = $value;
		      }
		    }
		  }
		  else {
		    $data = $item;
		  }
		  if (count($children) > 0) {
		    $data .= theme_item_list($children, NULL, $type, $attributes); // Render nested list
		  }
		  if ($i == 0) {
		    $attributes['class'] = empty($attributes['class']) ? 'first' : ($attributes['class'] . ' first');
		  }
		  if ($i == $num_items - 1) {
		    $attributes['class'] = empty($attributes['class']) ? 'last' : ($attributes['class'] . ' last');
		  }
		  $output .= '<li' . drupal_attributes($attributes) . '>' . $data . "</li>\n";
		}
		$output .= "</$type>";
	  }
	  $output .= '</div>';
	  return $output;
	}


	function theme_facetly_pager($results, $limit) {
	  if ($results > 0) {
	  	if ( !isset($_GET['page'] )) $_GET['page'] = 0;
		global $pager_total_items, $pager_total, $pager_page_array;
		$pager_total_items[0] = (int) $results;
		$pager_total[0] = ceil($results / $limit);
		$pager_page_array[0] = max(0, min((int) $_GET['page'], ($pager_total[0] - 1)));
	  }
	  $output = theme('pager');
	  return $output;
	}
<<<<<<< HEAD

	function theme_facetly_item($items) {
		static $facetly_field_image;
		$items_title = $items['title'];
		$items_body = $items['body'];
		$context = array(
			'revision' => 'themed', 
			'type' => 'product',
			'class' => array('product', 'sell_price'),
		);
		
		$items_price = uc_price($items['price'], $context);
		$output = "";

		if( empty($facetly_field_image) ){
			$facetly_field_key = variable_get('facetly_field_key', array());	
			foreach ( $facetly_field_key as $field_key => &$field_value ) {
				if( strpos($field_value, 'image') ) {
					$facetly_field_image = $field_key;				
				}
			}	
		}
		$image_path = $items[$facetly_field_image];
		$output .= theme('imagecache', 'product', $image_path, "", "");
		$items_id = $items['id'];
		$output .= "<br/>". l($items_title, 'node/'. $id = $items_id). "<br/>";	
		$output .= "". $items_body ."<br/>";
		$output .= "". $items_price ."<br/>";
		return $output; 
	}

function theme_facetly_search_form($form)
{
    static $counter;
    
    if (!isset($counter)) {
        $counter = 0;
    }
    
    facetly_add_js();
    drupal_add_css(drupal_get_path('module', 'facetly') . '/css/autocomplete.css');

    unset($form['form_build_id']);
    unset($form['form_token']);
    unset($form['form_id']);
    $output = drupal_render($form);
    $counter++;
    return $output;
}
=======

	function theme_facetly_item($items) {
		static $facetly_field_image;
		$items_title = $items['title'];
		$items_body = $items['body'];
		$context = array(
			'revision' => 'themed', 
			'type' => 'product',
			'class' => array('product', 'sell_price'),
		);
		
		$items_price = uc_price($items['price'], $context);
		$output = "";

		if( empty($facetly_field_image) ){
			$facetly_field_key = variable_get('facetly_field_key', array());	
			foreach ( $facetly_field_key as $field_key => &$field_value ) {
				if( strpos($field_value, 'image') ) {
					$facetly_field_image = $field_key;				
				}
			}	
		}
		$image_path = $items[$facetly_field_image];
		$output .= theme('imagecache', 'product', $image_path, "", "");
		$items_id = $items['id'];
		$output .= "<br/>". l($items_title, 'node/'. $id = $items_id). "<br/>";	
		$output .= "". $items_body ."<br/>";
		$output .= "". $items_price ."<br/>";
		return $output; 
	}

	function theme_facetly_search_form($form) {
	    facetly_add_js();
	    drupal_add_css(drupal_get_path('module', 'facetly') . '/css/autocomplete.css');

	    unset($form['form_build_id']);
	    unset($form['form_token']);
	    unset($form['form_id']);
	    $output = drupal_render($form);
	    return $output;
	}
>>>>>>> update drupal
