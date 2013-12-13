<?php
namespace Walkthrough;
use Guzzle\Http\Client;

class Connection {
  protected $authenticator;

  protected $endpoint;

  protected $drupalUser;

  public function login() {
    $this->drupalUser = $this->authenticator->login();

    return $this->drupalUser;
  }

  public function getScreeningQueue() {
    $client = $this->getClient();
    return $client->get('walkhub-walkthrough-screening-queue')->send()->json();
  }

  public function getPhpunit($uuid) {
    $client = $this->getClient();

    $response = $client->get('walkthrough-phpunit/' . $uuid)->send()->json();
    return $response[0];
  }

  /**
   * @param mixed $authenticator
   */
  public function setAuthenticator($authenticator) {
    $this->authenticator = $authenticator;
  }

  /**
   * Set the rest endpoint.
   *
   * @param string $endpoint
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * Prepare the Guzzle http client.
   *
   * Also populates the cookies if the authentication was successful.
   *
   * @return Guzzle\Http\Client
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
