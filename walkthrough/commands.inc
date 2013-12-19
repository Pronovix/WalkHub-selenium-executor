<?php

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

  echo "Ok.\n";
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
  $response = $connection->getPhpunit($command_line[1], $command_line[2]);

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

  // Create empty screening.

  // Remove screening flag.
  $connection->updateScreeningFlag($walkthrough_to_play['uuid'], FALSE);

  // Get phpunit output.
  $response = $connection->getPhpunit($walkthrough_to_play['type'], $walkthrough_to_play['uuid']);
  $phpunit_file = tempnam(sys_get_temp_dir(), 'wt-');
  $handle = fopen($phpunit_file, 'w');
  fwrite($handle, $response);
  fclose($handle);

  // Execute phpunit.
  system("phpunit $phpunit_file");
  unlink($phpunit_file);

  // Post results back.
}

/**
 * Command callback to flag/unflag a Walkthrough or a Walkthrough set.
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__flag(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $uuid = $command_line[1];
  $flag = $command_line[2];

  $connection->login();
  $connection->updateScreeningFlag($uuid, $flag);
}

/**
 *
 *
 * @param \Walkthrough\Connection $connection
 * @param ArrayAccess $command_line
 */
function command__upload_screenshot(\Walkthrough\Connection $connection, \ArrayAccess $command_line) {
  $uuid = $command_line[1];
  $screenshots = array(
    '/Users/hairqles/Pictures/profil.jpg',
  );
  $connection->login();
  $connection->uploadScreeningScreenshot($uuid, $screenshots);
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
