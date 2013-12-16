<?php

namespace Walkthrough\Authenticator;

abstract class WalkthroughAuthenticator {
  protected $endpoint;
  abstract public function login();

  /**
   * @param mixed $endpoint
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }
}