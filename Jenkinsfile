#!groovy

pipeline {

  agent {
    label 'docker-gpu-host'
  }

  options {
    timeout(time: 1, unit: 'HOURS')
  }  

  stages {
    stage('Build') {
      parallel {
        stage('gcc-6') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-6-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh './build.sh gcc-6 Release'
          }
          post {
            always {
              recordIssues enabledForFailure: true, aggregatingResults: false,
                tool: gcc(id: 'gcc-6', pattern: 'build-gcc-6/make.out')
            }
          }
        }
        stage('gcc-8') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-8-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh './build.sh gcc-8 Release'
          }
          post {
            always {
              recordIssues enabledForFailure: true, aggregatingResults: false,
                tool: gcc(id: 'gcc-8', pattern: 'build-gcc-8/make.out')
            }
          }
        }
        stage('clang-6') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-clang-6-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh './build.sh clang-6 Release'
          }
          post {
            always {
              recordIssues enabledForFailure: true, aggregatingResults: false,
                tool: clang(id: 'clang-6', pattern: 'build-clang-6/make.out')
            }
          }
        }
        stage('clang-8') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-clang-8-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh './build.sh clang-8 Release'
          }
          post {
            always {
              recordIssues enabledForFailure: true, aggregatingResults: false,
                tool: clang(id: 'clang-8', pattern: 'build-clang-8/make.out')
            }
          }
        }
      }
    }
    stage('Test') {
      parallel {
        stage('gcc-6') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-6-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh 'cd build-gcc-6 && make test'
          }
          post {
            always {
              step([
                $class: 'XUnitPublisher',
                thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
                tools: [[$class: 'GoogleTestType', pattern: 'build-gcc-6/Testing/*.xml']]
              ])
            }
          }
        }
        stage('gcc-8') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-8-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh 'cd build-gcc-8 && make test'
          }
          post {
            always {
              step([
                $class: 'XUnitPublisher',
                thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
                tools: [[$class: 'GoogleTestType', pattern: 'build-gcc-8/Testing/*.xml']]
              ])
            }
          }
        }
        stage('clang-6') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-clang-6-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh 'cd build-clang-6 && make test'
          }
          post {
            always {
              step([
                $class: 'XUnitPublisher',
                thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
                tools: [[$class: 'GoogleTestType', pattern: 'build-clang-6/Testing/*.xml']]
              ])
            }
          }
        }
        stage('clang-8') {
          agent {
            docker {
              reuseNode true
              image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-clang-8-conan-1.19'
              args '--runtime=nvidia'
            }
          }
          steps {
            sh 'cd build-clang-8 && make test'
          }
          post {
            always {
              step([
                $class: 'XUnitPublisher',
                thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
                tools: [[$class: 'GoogleTestType', pattern: 'build-clang-8/Testing/*.xml']]
              ])
            }
          }
        }
      }
    }
    stage('Doxygen') {
      agent {
        docker {
          reuseNode true
          image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-7-conan-1.19-doxygen-1.8.13'
          args '--runtime=nvidia'
        }
      }
      steps {
        sh './build.sh doc Release'
        publishHTML( target: [
          allowMissing: false,
          alwaysLinkToLastBuild: false,
          keepAll: true,
          reportName: 'Doxygen',
          reportDir: 'build-doc/doxygen/html',
          reportFiles: 'index.html'
        ])
      }
    }
    stage('Coverage') {
      agent {
        docker {
          reuseNode true
          image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-gcc-8-conan-1.19'
          args '--runtime=nvidia'
        }
      }
      steps {
        sh '''
          ./build.sh gcc-8-cov Coverage
          cd build-gcc-8-cov
          make test
          gcovr -r .. -x > coverage.xml
        '''
      }
      post {
        always {
          step([
            $class: 'CoberturaPublisher',
            autoUpdateHealth: false, autoUpdateStability: false, coberturaReportFile: '**/coverage.xml',
            failUnhealthy: false, failUnstable: false, maxNumberOfBuilds: 0, onlyStable: false,
            sourceEncoding: 'ASCII', zoomCoverageChart: false
          ])
        }
      }
    }
    stage('Softwipe') {
      agent {
        docker {
          reuseNode true
          image 'braintwister/ubuntu-18.04-cuda-10.1-cmake-3.15-clang-7-conan-1.19-softwipe-fork-18a7313'
          args '--runtime=nvidia'
        }
      }
      steps {
        sh '''
          mkdir -p conan-softwipe
          export CONAN_USER_HOME=$PWD/conan-softwipe
          rm -fr build-softwipe
          mkdir -p build-softwipe
          cd build-softwipe
          /softwipe/softwipe.py -CM .. -e ../run_softwipe.sh -x /usr,/tmp,../conan-
        '''
      }
    }
    stage('Deploy') {
      agent {
        dockerfile {
          reuseNode true
          filename 'devel/Dockerfile-deploy'
          args '--runtime=nvidia'
        }
      }
      steps {
        sh '''
          export CONAN_USER_HOME=$PWD/conan-gcc-8
          cd build-gcc-8
          make package
        '''
      }
      post {
        success {
          archiveArtifacts artifacts: "build*/Pink*", fingerprint: true
        }
      }
    }
  }
  post {
    success {
      mail to: 'bernd.doser@h-its.org', subject: "SUCCESS: ${currentBuild.fullDisplayName}", body: "All fine."
    }
    failure {
      mail to: 'bernd.doser@h-its.org', subject: "FAILURE: ${currentBuild.fullDisplayName}", body: "Failed."
    }
  }
}
