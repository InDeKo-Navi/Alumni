<?php

function display_userprofile($user_id) {

    global $user;
    $content = array();

    $fields = array('image_id', 'first_name', 'last_name', 'jufo_projects', 'other_projects', 'visited_schools', 'city', 'zip_code', 'telephone', 'other_profiles', 'check_email', 'check_post', 'check_telephone', 'birthday_day', 'birthday_month', 'birthday_year', 'gender', 'hobbies', 'interests', 'received_awards', 'organisations');

    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    $tabsHTML =
        '<div class="tab">' .
        '<a class="tablinks active" target="general">' . t('Allgemein') . '</a>' .
        '<a class="tablinks" target="contact-preference">' . t('Kontaktpräferenz') . '</a>' .
        '<a class="tablinks" target="personal">' . t('Persönlich') . '</a>' .
        '<a class="tablinks" target="others">' . t('Sonstiges') . '</a>' .
        '</div>';

    $content['tabarea'] = array(
        '#markup' => $tabsHTML,
    );

    $content['general_start'] = array(
        '#markup' => '<div id="general" class="tabcontent">',
    );


    //Nutzerprofil Allgemein

    $content['general_headline'] = array(
        '#markup' => '<h3>' . t('Allgemein & Kontaktdaten') . '</h3>',
    );

    $imageText = '';
    if ($result['image_id'] != 0) {
        $image = file_load($result['image_id']);


        $variables = array(
            'path' => $image->uri,
            'alt' => t('Profilbild'),
            'title' => t('Profilbild'),
            'width' => '200px',
        );
        $imageText = theme('image', $variables);

    } else {
        $imageText = '<div class="picture-alternative">' . t('Kein Profilbild angegeben.') . '</div>';
    }

    $content['general_left_container'] = array(
        '#markup' => '<div class="general-left-container">' . $imageText . '</div>'
    );

    $jfProjectsHTML = t('- keine Angabe -');

    if (!empty($result['jufo_projects'])) {
        $jfProjectsHTML = '';
        $jfProjects = unserialize($result['jufo_projects']);

        for ($i = 0; $i < count($jfProjects); $i++) {
            $jfProjectsHTML .= '<span class="profile-value">' . $jfProjects[$i] . '</span><br>';
        }

        if (count($jfProjects) == 0) {
            $jfProjectsHTML = t('- keine Angabe -');
        }

    }
    $otherProjectsHTML = t('- keine Angabe -');

    if (!empty($result['other_projects'])) {
        $otherProjectsHTML = '';
        $otherProjects = unserialize($result['other_projects']);

        for ($i = 0; $i < count($otherProjects); $i++) {
            $otherProjectsHTML .= '<span class="profile-value">' . $otherProjects[$i] . '</span><br>';
        }

        if (count($otherProjects) == 0) {
            $otherProjectsHTML = t('- keine Angabe -');
        }

    }
    $visitedSchoolsHTML = t('- keine Angabe -');

    if (!empty($result['visited_schools'])) {
        $visitedSchoolsHTML = '';
        $visitedSchools = unserialize($result['visited_schools']);

        for ($i = 0; $i < count($visitedSchools); $i++) {
            $visitedSchoolsHTML .= '<span class="profile-value">' . $visitedSchools[$i] . '</span><br>';
        }

        if (count($visitedSchools) == 0) {
            $visitedSchoolsHTML = t('- keine Angabe -');
        }
    }

    $username = (empty($result['first_name']) && empty($result['last_name'])) ? t('- keine Angabe -') : $result['first_name'] . ' ' . $result['last_name'];

    $HTML =
        '<div id="sep1" class="v-separator"></div>' .
        '<div class="general-middle-container">' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Name') . ':</span><br>' .
        '<span class="profile-value">' . $username . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Jugend Forscht Projekte') . ':</span><br>' .
        $jfProjectsHTML .
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Weitere Projekte') . ':</span><br>' .
        $otherProjectsHTML .
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Besuchte Schulen') . ':</span><br>' .
        $visitedSchoolsHTML .
        '</p>' .
        '</div>';

    $content['general_middle_container'] = array(
        '#markup' => $HTML,
    );


    $city = (empty($result['city'])) ? t('- keine Angabe -') : $result['city'];
    $zip_code = (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : t('- keine Angabe -');
    $telephone = (empty($result['telephone'])) ? t('- keine Angabe -') : $result['telephone'];

    $otherProfilesHTML = t('- keine Angabe -');

    if (!empty($result['other_profiles'])) {
        $otherProfilesHTML = '';
        $otherProfiles = unserialize($result['other_profiles']);

        foreach ($otherProfiles as $key => $value) {
            $otherProfilesHTML .= '<span class="profile-value"><a href="' . $value . '" target="_blank">' . $key . '</a></span><br>';
        }

        if (count($otherProfiles) == 0) {
            $otherProfilesHTML = t('- keine Angabe -');
        }
    }

    $HTML =
        '<div id="sep2" class="v-separator"></div>' .
        '<div class="general-right-container">' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Stadt') . ':</span><br>' .
        '<span class="profile-value">' . $city . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('PLZ') . ':</span><br>' .
        '<span class="profile-value">' . $zip_code . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Telefon/Mobil') . ':</span><br>' .
        '<span class="profile-value">' . $telephone . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Profile') . ':</span><br>' .
        $otherProfilesHTML .
        '</p>' .
        '</div>';

    $content['general_right_container'] = array(
        '#markup' => $HTML,
    );

    $content['general_end'] = array(
        '#markup' => '<div class="clear-float"></div></div>',
    );

    //Nutzerprofil Kontaktpräferenz

    $content['contactpreference_start'] = array(
        '#markup' => '<div id="contact-preference" class="tabcontent">',
    );

    $checkEmail = ($result['check_email'] == 0) ? '' : '<span class="profile-value">' . t('E-Mail'). '</span><br>';
    $checkPost = ($result['check_post'] == 0) ? '' : '<span class="profile-value">' . t('Post'). '</span><br>';
    $checkTelephone = ($result['check_telephone'] == 0) ? '' : '<span class="profile-value">' . t('Telefon/Mobil'). '</span><br>';
    $checkAll = ($result['check_email'] == 0 && $result['check_post'] == 0 && $result['check_telephone'] == 0) ? t('- keine Angaben -') : '';

    $HTML =
        '<span class="profile-bold-text">' . t('Benutzer darf über folgende Kanäle kontaktiert werden') . ':</span>' .
        '<p class="profile-container-contact">' .
        $checkAll .
        $checkEmail .
        $checkPost .
        $checkTelephone .
        '</p>';

    $content['contact_container'] = array(
        '#markup' => $HTML,
    );

    $content['contactpreference_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Persönlich
    $content['personal_start'] = array(
        '#markup' => '<div id="personal" class="tabcontent">',
    );

    $birthday = (empty($result['birthday_day']) && empty($result['birthday_month'] && empty($result['birthday_year'])) == 0) ? t('- keine Angabe -') : $result['birthday_day'] . ' ' . $result['birthday_month'] . ' ' . $result['birthday_year'];
    $gender = ($result['gender'] == null) ? t('- keine Angabe -') : $result['gender'];

    $hobbiesHTML = t('- keine Angabe -');

    if (!empty($result['hobbies'])) {
        $hobbiesHTML = '';
        $hobbies = unserialize($result['hobbies']);

        foreach ($hobbies as $hobbie) {
            $hobbiesHTML .= '<span class="profile-value">' . $hobbie . '</a></span><br>';
        }

        if (count($hobbies) == 0) {
            $hobbiesHTML = t('- keine Angabe -');
        }
    }

    $interestsHTML = t('- keine Angabe -');

    if (!empty($result['interests'])) {
        $interestsHTML = '';
        $interests = unserialize($result['interests']);

        foreach ($interests as $interest) {
            $interestsHTML .= '<span class="profile-value">' . $interest . '</a></span><br>';
        }

        if (count($interests) == 0) {
            $interestsHTML = t('- keine Angabe -');
        }
    }

    $HTML =
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Geburtsdatum') . ':</span><br>' .
        '<span class="profile-value">' . $birthday . '</span>'.
        '</p>' .
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Geschlecht') . ':</span><br>' .
        '<span class="profile-value">' . $gender . '</span>'.
        '</p>' .
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Hobbys') . ':</span><br>' .
        $hobbiesHTML .
        '</p>'.
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Interessen') . ':</span><br>' .
        $interestsHTML .
        '</p>';

    $content['personal_container'] = array(
        '#markup' => $HTML,
    );

    $content['personal_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Others
    $content['others_start'] = array(
        '#markup' => '<div id="others" class="tabcontent">',
    );

    $awardsHTML = t('- keine Angabe -');

    if (!empty($result['interests'])) {
        $awardsHTML = '';
        $awards = unserialize($result['received_awards']);

        foreach ($awards as $award) {
            $awardsHTML .= '<span class="profile-value">' . $award . '</a></span><br>';
        }

        if (count($awards) == 0) {
            $awardsHTML = t('- keine Angabe -');
        }
    }

    $organisationsHTML = t('- keine Angabe -');

    if (!empty($result['organisations'])) {
        $organisationsHTML = '';
        $organisations = unserialize($result['organisations']);

        foreach ($organisations as $organisation) {
            $organisationsHTML .= '<span class="profile-value">' . $organisation . '</a></span><br>';
        }

        if (count($organisations) == 0) {
            $organisationsHTML = t('- keine Angabe -');
        }
    }

    $HTML =
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Erhaltene Auszeichnungen') . ':</span><br>' .
        $awardsHTML .
        '</p>'.
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Organisationen') . ':</span><br>' .
        $organisationsHTML .
        '</p>';

    $content['others_container'] = array(
        '#markup' => $HTML,
    );

    $content['others_end'] = array(
        '#markup' => '</div>',
    );


    if ($user_id == $user->uid || user_has_role(3, user_load($user->uid))) {
        $content['edit_button'] = array(
            '#markup' => '<br><a class="btn edit-userprofile" href="' . base_path() . 'user/' . $user_id . '/userprofile/edit">' . t('Editieren') . '</a>',
        );
    }

    return $content;
}