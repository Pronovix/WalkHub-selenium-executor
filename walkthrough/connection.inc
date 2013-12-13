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
    $client = new Client($this->endpoint);
    return $client->get('walkhub-walkthrough-screening-queue')->send()->json();
  }

  public function getPhpunit($uuid) {
    $client = new Client($this->endpoint);

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

}
