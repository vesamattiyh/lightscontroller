import { Image, StyleSheet, Text, View } from 'react-native'

import LogoImg from "../assets/images/react-logo.png"


const Logo = () => {
  return (
    <View style={styles.container}>
        <Image source={LogoImg} style={styles.img}/>
        <Text style={styles.title}>Lights Controller</Text>
    </View>
  )
}

export default Logo

const styles = StyleSheet.create({
 
    container : {
        flexDirection: 'row',
        position: 'absolute',
        top: 75,
        alignContent:'center',
        alignItems: 'center'
    },
 
    title : {
        fontSize: 25,
        fontWeight: 'bold',
        color : '#FFF'
  },

  img: {
    marginRight:10
}
})