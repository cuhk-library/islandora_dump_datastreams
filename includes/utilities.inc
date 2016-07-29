<?php

/**
 * @file
 * Utilities.
 */

/**
 * Returns a list of content models that this module supports.
 *
 * @return array
 *   A list of supported content model PIDs.
 */
function islandora_dump_datastreams_get_supported_cmodels() {
  return array(
    'islandora:bookCModel',
    // 'islandora:compoundCModel',
    'islandora:newspaperIssueCModel',
    'islandora:sp-audioCModel',
    'islandora:sp_basic_image',
    'islandora:sp_disk_image',
    'islandora:sp_large_image_cmodel',
    'islandora:sp_pdf',
    'islandora:sp_videoCModel',
    'islandora:sp_web_archive',
  );
}

/**
 * Populates a list of cmodels for use in the admin setting form.
 *
 * Filters out content models that are not installed.
 *
 * @return array
 *   A list of supported content models that are currently installed.
 */
function islandora_dump_datastreams_get_cmodels() {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $cmodels = islandora_get_content_models();
  $options = array();
  foreach ($cmodels as $cmodel) {
    $pid = $cmodel['pid'];
    $options[$pid] = $cmodel['label'];
  }
  $supported_cmodels = islandora_dump_datastreams_get_supported_cmodels();
  $options = array_intersect(array_keys($options), $supported_cmodels);
  return $options;
}

/**
 * Returns a list of DSIDs that we don't want to dump.
 *
 * @return array
 *   A list of DSIDs that we don't want to dump.
 */
function islandora_dump_datastreams_dsids_to_skip() {
  $skip = variable_get('islandora_dump_datastreams_skip_dsids', 'RELS-EXT');
  $skip = explode(',', $skip);
  $dsids_to_skip = array();
  foreach ($skip as &$dsid) {
    $dsid = trim($dsid);
    $dsids_to_skip[] = $dsid;
  }
  return $dsids_to_skip;
}

/**
 * Creates the object-level output directory for single-file objects.
 *
 * @param AbstractObject $object
 *   The object that has just been ingested.
 */
function islandora_dump_datastreams_dump_single(AbstractObject $object) {
  $base_output_dir = variable_get('islandora_dump_datastreams_output_dir', '/tmp');
  $dsids_to_skip = islandora_dump_datastreams_dsids_to_skip();
  if (!is_writable($base_output_dir)) {
    return;
  }
  foreach ($object as $ds) {
    if (!in_array($ds->id, $dsids_to_skip)) {
      $mime_detect = new MimeDetect();
      $extension = $mime_detect->getExtension($ds->mimetype);
      $filename = $ds->id . '.' . $extension;

      $object_dir_name = preg_replace('/:/', '_', $object->id);
      $output_dir = $base_output_dir . DIRECTORY_SEPARATOR . $object_dir_name;
      if (!file_exists($output_dir)) {
        // Loose permissions but probably not a huge security risk. There is a
        // note in the admin form indicating this.
        mkdir($output_dir);
      }
      $ds_output_path = $output_dir . DIRECTORY_SEPARATOR . $filename;

      $ds->getContent($ds_output_path);
    }
  }
}

/**
 * Creates the object-level output directory for newspaper issue objects.
 *
 * @param AbstractObject $object
 *   The object that has just been ingested.
 */
function islandora_dump_datastreams_dump_newspaper(AbstractObject $object) {
  // @todo: Get pages and write them out in the structure required by
  // the Newspaper Batch module.
}

/**
 * Creates the object-level output directory for book objects.
 *
 * @param AbstractObject $object
 *   The object that has just been ingested.
 */
function islandora_dump_datastreams_dump_book(AbstractObject $object) {
  // @todo: Get pages and write them out in the structure required by
  // the Book Batch module.
}

/**
 * Writes a datastream file to the output directory.
 *
 * @param AbstractObject $object
 *   The object that has just been ingested.
 * @param AbstractObject $datastream
 *   The datastream that has just been ingested.
 */
function islandora_dump_datastreams_dump_datastream(AbstractObject $object, AbstractObject $datastream) {
  $dsids_to_skip = islandora_dump_datastreams_dsids_to_skip();
  if (in_array($datastream->id, $dsids_to_skip)) {
    return;
  }

  $base_output_dir = variable_get('islandora_dump_datastreams_output_dir', '/tmp');
  if (!is_writable($base_output_dir)) {
    return;
  }

  $mime_detect = new MimeDetect();
  $extension = $mime_detect->getExtension($datastream->mimetype);
  $filename = $datastream->id . '.' . $extension;

  $object_dir_name = preg_replace('/:/', '_', $object->id);
  $output_dir = $base_output_dir . DIRECTORY_SEPARATOR . $object_dir_name;
  if (!file_exists($output_dir)) {
    // Loose permissions but probably not a huge security risk. There is a
    // note in the admin form indicating this.
    mkdir($output_dir);
  }
  $ds_output_path = $output_dir . DIRECTORY_SEPARATOR . $filename;

  $datastream->getContent($ds_output_path);
}