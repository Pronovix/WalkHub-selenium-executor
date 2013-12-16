<?php

function command__status(\Walkthrough\Connection $connection, $command_line) {
  try {
    $user = $connection->login();
  } catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    echo format_error($e->getResponse());
    exit(1);
  }

  if ($command_line['debug']) {
    echo "User:\n";
    var_dump($user);
  }

  echo "Ok.\n";
}

function command__get_queue(\Walkthrough\Connection $connection, $command_line) {
  try {
    $connection->login();
    $response = $connection->getScreeningQueue();
  } catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    echo format_error($e->getResponse());
    exit(1);
  }
  var_dump($response);
}

function command__get_phpunit(\Walkthrough\Connection $connection, $command_line) {
  try {
    $connection->login();
    $response = $connection->getPhpunit($command_line[1], $command_line[2]);
  } catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    echo format_error($e->getResponse());
    exit(1);
  }

  echo $response;
}

function command__process_queue(\Walkthrough\Connection $connection, $command_line) {
  try {
    $connection->login();

    // Set flag for processing.

    // Query queue.
    $queue = $connection->getScreeningQueue();
    if (empty($queue)) {
      echo "Queue is empty.\n";
      exit();
    }

    $walkthrough_to_play = array_pop($queue);

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

    // Set flag that it's ready.

  } catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    echo format_error($e->getResponse());
    exit(1);
  }
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
