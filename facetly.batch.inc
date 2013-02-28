<?php
	function facetly_batch() {
		$node_type = variable_get('facetly_node_type', array());
	  	$operations = array();
	  	foreach ($node_type as $key => $value) {
	  		if ( $value != "0" ) {
	  			$query_node = db_query("SELECT nid FROM {node} WHERE type = '%s' AND status=1 ORDER BY nid ASC", $value);
				while ( $get_nid = db_fetch_object($query_node) ) {
					$operations[] = array('facetly_batch_node_operation', array($get_nid->nid));
				}
	  		}
		}
		$batch = array(
			'operations' => $operations, 
			'finished' => 'facetly_batch_finished',
			'file' => drupal_get_path('module','facetly')  . '/facetly.batch.inc',
		);
		return $batch;
	}

	function facetly_batch_finished($success, $results, $operations) {
		if ($success) {
			$message = count($results) . ' processed.';
			drupal_set_message($message);
		}
		else {
			$error_operation = reset($operations);
			$message = $_SESSION['facetly_reindex'];
			unset($_SESSION['facetly_reindex']);
			drupal_set_message($message, 'error', TRUE);
		}
	}	
	
	function facetly_batch_node_operation($nid, &$context) {		
		module_load_include('inc','facetly','facetly.common');
		$node = node_load($nid);
		try {		
			facetly_reindex($node, TRUE);
		} catch (Exception $e) {
           $message = $e->getMessage();
		}
		if ($message != "") {
			$_SESSION['facetly_reindex'] = $message;
			break;
		}
		// Store some result for post-processing in the finished callback.
		$context['results'][] = $node->nid . ' : ' . check_plain($node->title);

		// Optional message displayed under the progressbar.
		$context['message'] = t('Loading @title', array('@title' => $node->title));
	}
	
	
