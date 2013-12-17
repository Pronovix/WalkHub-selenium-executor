<?php

namespace Walkthrough\Authenticator;
use Guzzle\Http\Client;

abstract class WalkthroughAuthenticator {
  protected $endpoint;
  protected $drupalUser;

  /**
   * Log in to Drupal.
   *
   * @return \stdClass
   * Logged in Drupal user object.
   */
  abstract public function login();

  /**
   * @param string $endpoint
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * @return string
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  /**
   * Prepare the Guzzle http client.
   *
   * Also populates the cookies if the authentication was successful.
   *
   * @return \Guzzle\Http\Client
   */
  public function getClient() {
    if ($this->drupalUser['user']['uid'] == 0) {
      return new Client($this->endpoint);
    }

    $client = new Client($this->endpoint, array(
      'request.options' => array(
        'cookies' => array($this->drupalUser['session_name'] => $this->drupalUser['sessid']),
      ),
    ));

    return $client;
  }

}