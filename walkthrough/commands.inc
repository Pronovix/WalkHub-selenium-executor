<?php

/**
 * Walkthrough selenium return statuses.
 *
 * These are the possible values of the field_screening_result field in the screening content type.
 */
define('WALKTHROUGH_SELENIUM_RESULT_SUCCESS', 'Successful');
define('WALKTHROUGH_SELENIUM_RESULT_TIMEOUT', 'Timeout');
define('WALKTHROUGH_SELENIUM_RESULT_BROKEN', 'Broken');

/**
 * Command callback to check the connection status.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__status(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $user = $connection->login();

  if ($command_line['debug']) {
    echo "User:\n";
    var_dump($user);
  }

  if (!empty($user)) {
    echo "Ok.\n";
    exit(0);
  }

  echo "Error.\n";
  exit(1);
}

/**
 * Command callback to get the sreening queue.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__get_queue(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $connection->login();
  $response = $connection->getScreeningQueue();

  var_dump($response);
}

/**
 * Command callback to get a phpunit export for a walkthrough or walkthrough
 * suite.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__get_phpunit(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $connection->login();
  $response = $connection->getPhpunit($command_line[1], $command_line[2], $command_line['extend_custom_class']);

  echo $response;
}

/**
 * Command callback to process the screening queue.
 *
 * Get the first element, unset the screening flag, run the phpunit test and
 * post back the results.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__process_queue(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $connection->login();

  // Query queue.
  $queue = $connection->getScreeningQueue();
  if (empty($queue)) {
    echo "Queue is empty.\n";
    exit();
  }
  $walkthrough_to_play = array_pop($queue);

  // Remove screening flag.
  $connection->updateScreeningFlag($walkthrough_to_play['uuid'], FALSE);

  // Get phpunit output.
  $response = $connection->getPhpunit($walkthrough_to_play['type'], $walkthrough_to_play['uuid'], $command_line['extend_custom_class']);
  $phpunit_file = tempnam(sys_get_temp_dir(), 'wt-');
  $handle = fopen($phpunit_file, 'w');
  fwrite($handle, $response);
  fclose($handle);

  // Prepare screenshot directory.
  $screenshot_dir = uniqid(sys_get_temp_dir() . '/wtscreenshot');
  mkdir($screenshot_dir, 0700);

  $junit_log_file = tempnam(sys_get_temp_dir(), 'wt-junit-');

  // Prepare environment variables.
  $environment_array = array(
    'WT_SCREENSHOT_PATH' => $screenshot_dir . '/',
  );
  if ($command_line['browser']) {
    $environment_array += array('WT_BROWSER' => $command_line['browser']);
  }
  $environment = '';
  foreach ($environment_array as $variable => $setting) {
    $environment .= $variable . '="' . $setting . '" ';
  }

  // Execute phpunit.
  $return_value = 0;
  $time_start = microtime(TRUE);
  system("$environment ./vendor/bin/phpunit  --bootstrap bootstrap.inc --log-junit $junit_log_file $phpunit_file", $return_value);
  $time_stop = microtime(TRUE);
  $elapsed_time = $time_stop - $time_start;
  unlink($phpunit_file);

  // Post results back.
  $screening_uuid = $connection->createEmptyScreening($walkthrough_to_play['uuid']);
  $connection->sendScreeningResult($screening_uuid, array('result' => (($return_value == 0) ? WALKTHROUGH_SELENIUM_RESULT_SUCCESS : WALKTHROUGH_SELENIUM_RESULT_BROKEN)));
  $connection->sendScreeningResult($screening_uuid, array('junit' => file_get_contents($junit_log_file)));
  $connection->sendScreeningResult($screening_uuid, array('time' => $elapsed_time));
  upload_screenshots($connection, $screenshot_dir, $screening_uuid);

  // Clean up screenshot files.
  foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator($screenshot_dir, FilesystemIterator::SKIP_DOTS), RecursiveIteratorIterator::CHILD_FIRST) as $path) {
    $path->isFile() ? unlink($path->getPathname()) : rmdir($path->getPathname());
  }
  rmdir($screenshot_dir);
}

/**
 * Helper function to upload screenshots from a directory to a screening content type.
 *
 * @param \Walkthrough\Connection $connection
 *   Walkthrough connection object.
 * @param string $directory
 *   Directory containing the screenshots.
 * @param string $screening_uuid
 *   Screening uuid to upload the screening to.
 */
function upload_screenshots(\Walkthrough\Connection $connection, $directory, $screening_uuid) {
  $screenshots = array();

  $iterator = new DirectoryIterator($directory);
  foreach ($iterator as $path) {
    if ($path->isDot()) {
      continue;
    }

    list($nid, $step_id) = explode(':', $path->getFilename());
    $screenshots[(int) $step_id] = $directory . DIRECTORY_SEPARATOR . $path->getFilename();
  }

  // Upload screenshots in the order of steps for simplicity.
  ksort($screenshots);

  foreach ($screenshots as $step_id => $filename) {
    $connection->uploadScreeningScreenshot($screening_uuid, $filename, $step_id);
  }
}

/**
 * Command callback to flag/unflag a Walkthrough or a Walkthrough set.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__flag(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $uuid = $command_line[1];
  $flag = isset($command_line[2]) ? $command_line[2] : TRUE;

  $connection->login();
  $connection->updateScreeningFlag($uuid, $flag);
}

/**
 * Formats a Guzzle error response to the command line.
 *
 * @param \Guzzle\Http\Message\Response $response
 * @return string
 */
function format_error(Guzzle\Http\Message\Response $response) {
  return 'Error: [' . $response->getStatusCode() . '] ' . $response->getReasonPhrase() . "\n";
}
