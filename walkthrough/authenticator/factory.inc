<?php

namespace Walkthrough\Authenticator;

use Walkthrough\Authenticator;


class AuthenticatorFactory {

  /**
   * Factory for creating an Authenticator object.
   *
   * @param $settings \ArrayAccess
   *   Settings array to build the authenticator from. Probably set via
   *   Commando.
   *   Must include Walkhub path via "walkhub_url".
   *
   *   Authentication set by the following rules:
   *   - If "username" and "password" present, it creates a Basic HTTP
   *     authentication object.
   *   - If "consumer secret" and "consumer key" present, it creates an Oauth
   *     authentication.
   *   - If neither is present it creates an Anonymous authenticator object.
   *
   * @return WalkthroughAuthenticator
   *
   * @throws NotAuthenticatedException
   */
  public static function create(\ArrayAccess $settings) {
    $endpoint = $settings['walkhub_url'] . '/api/v2';

    $authenticator = NULL;

    // Basic HTTP authentication.
    if ($settings['username'] && $settings['password']) {
      include_once 'walkthrough/authenticator/basic.inc';

      $authenticator = new Basic();
      $authenticator->setUsername($settings['username']);
      $authenticator->setPassword($settings['password']);
    }
    elseif ($settings['consumer key'] && $settings['consumer secret']) {
      // Oauth authentication.
      include_once 'walkthrough/authenticator/oauth.inc';

      $authenticator = new Oauth();
      $authenticator->setConsumerKey($settings['consumer key']);
      $authenticator->setConsumerSecret($settings['consumer secret']);

      $endpoint .= '/oauth';
    }

    // If we don't have authentication we fall back to Anonymous session.
    if ($authenticator === NULL) {
      throw new NotAuthenticatedException();
    }

    $authenticator->setEndpoint($endpoint);


    return $authenticator;
  }
}
