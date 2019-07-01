pipeline {
  agent {
    dockerfile {
      filename 'source/Dockerfile'
      label 'docker-host-dev'
    }
  }
  options {
    buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '30'))
    checkoutToSubdirectory('source')
    timeout(time: 1, unit: 'HOURS')
  }

  stages {
    stage('CMake') { steps {
      dir("${env.WORKSPACE}/build") {
        sh '''cmake -DOSG_DIR=C:/install/OpenSceneGraph-3.6.3-VC2017-64-Release/ -DCMAKE_INSTALL_PREFIX=../install ../source'''
      }
    } }
    stage('Build') {
      steps {
        dir("${env.WORKSPACE}/build") {
          sh '''make install'''
        }
      }
    }
    stage('Artifacts') {
      steps {
          sh '''tar -cvzf OSGLauncher.tar.gz install/*'''
archiveArtifacts artifacts: 'OSGLauncher.tar.gz', fingerprint: true, onlyIfSuccessful: true
      }
    }
  }

  post {
    always {
emailext attachLog: true, body: '''${DEFAULT_CONTENT}''', compressLog: true, recipientProviders: [[$class: 'CulpritsRecipientProvider']], subject: '${DEFAULT_SUBJECT}', to: '${DEFAULT_RECIPIENTS}'
    }
  }
}

